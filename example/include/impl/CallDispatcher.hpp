#ifndef CALL_DISPATCHER
#define CALL_DISPATCHER

#include <cstddef>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <exception>
#include <string>
#include <thread>
#include <tuple>

#include "impl/MsgPacker.hpp"
#include "impl/TcpConnector.hpp"

namespace dispatcher {
	enum CallState {
			PENDING,
			DONE
	};

	class CallInterface {
		public:
			virtual void fulfillPromise(const nvimRpc::packer::PackedRequestResponse& packedResponse) = 0;
			virtual CallState state() = 0;
	};

	template<class T, class... U> class Call: public CallInterface {
		private: 
			CallState _state;
			const char* _data;
			size_t _dataSize;
			std::promise<T> _promise;

		public:
			Call<T, U...>(const std::shared_ptr<nvimRpc::packer::PackedRequest<U...>>& request) {
				_state = PENDING;
				_data = request->data();
				_dataSize = request->size();
			}

			CallState state() {
				return _state;
			}

			const char* data() {
				return _data;
			}

			size_t size() {
				return _dataSize;
			}

			std::future<T> getFuture() {
				return _promise.get_future();
			}

			void fulfillPromise(const nvimRpc::packer::PackedRequestResponse& packedResponse) {
				T value;
				nvimRpc::packer::Error error;

				if (packedResponse.error(error)) {
					_promise.set_exception(std::make_exception_ptr(std::runtime_error(std::get<1>(error))));
				} else if (packedResponse.value(value)) {
					_promise.set_value(value);
				} else {
					// this is for when fulfilling a promise to Void, packedResponse.value() returns false
					_promise.set_value(T());
				}

				_state = DONE;
			}
	};

	class CallDispatcher {
		private:
			std::mutex* _callMap_mtx;
			std::mutex* _connector_mtx;
			const Tcp::Connector* _connector;
			std::map<int, CallInterface*> _callMap;
			std::thread* _thread;

			std::vector<char> _readFromSocket() {
				std::lock_guard lockConnector(*_connector_mtx);

				if (!_connector->available()) {
					return std::vector<char>();
				}
				return _connector->read();
			}

			bool _isConnectorConnected() {
				std::lock_guard lockConnector(*_connector_mtx);

				return _connector->isConnected();
			}

			void _unpackReceivedMessage(const std::vector<char>& receivedMessage) {
				size_t offset = 0;

				while (offset != receivedMessage.size()) {
					auto packedResponse = nvimRpc::packer::PackedRequestResponse(receivedMessage, offset);

					switch (packedResponse.type()) {
						case nvimRpc::packer::MessageType::RESPONSE:
							_fulfillPlacedCall(packedResponse);
							break;
						case nvimRpc::packer::MessageType::NOTIFY:
							//figure out what to do with notifications
							throw std::runtime_error("Notifications handling has not been implemented yet because I'm a lazy fuck");
					}
				}
			}

			void _fulfillPlacedCall(const nvimRpc::packer::PackedRequestResponse& packedResponse) {
				std::lock_guard lockCallMap(*_callMap_mtx);

				auto call = _callMap[packedResponse.id()];
				if (call->state() == DONE) {
					throw std::runtime_error(std::string("request with id ") + std::to_string(packedResponse.id()) + std::string(" already fulfilled"));
				}

				call->fulfillPromise(packedResponse);
				// remove call from map?
			}


		public:
			CallDispatcher(const Tcp::Connector* connector): _connector(connector) {
				_callMap = std::map<int, CallInterface*>();
				_callMap_mtx = new std::mutex();
				_connector_mtx = new std::mutex();
				_thread = NULL;
			}

			template <typename T, typename ...U>
			std::future<T> placeCall(const std::shared_ptr<nvimRpc::packer::PackedRequest<U...>> request) {
				std::lock_guard lockCallMap(*_callMap_mtx);
				std::lock_guard lockConnector(*_connector_mtx);
				Call<T, U...>* callToPlace = new Call<T, U...>(request);
				_callMap[request->id()] = callToPlace;
				_connector->send(callToPlace->data(), callToPlace->size());

				return callToPlace->getFuture();
			}

			void listenToConnector() {
				nvimRpc::packer::MessageIdentifier messageIdentifier;
				while (_isConnectorConnected()) {
					auto readFromSocket = _readFromSocket();

					if (readFromSocket.size() > 0) {
						_unpackReceivedMessage(readFromSocket);
					}
				}
			}

			static void makeCallDistacherListen(CallDispatcher* callDispatcher) {
				callDispatcher->listenToConnector();
			}

			static std::thread startCallDispatcher(CallDispatcher* callDispatcher) {
				std::thread t(makeCallDistacherListen, callDispatcher);

				return t;
			}
	};
}

#endif /* !CALL_DISPATCHER */
