#ifndef CALL_DISPATCHER
#define CALL_DISPATCHER

#include <cstddef>
#include <future>
#include <mutex>
#include <exception>
#include <thread>

#include "impl/MsgPacker.hpp"
#include "impl/TcpConnector.hpp"

namespace dispatcher {
	enum CallState {
			PENDING,
			DONE
	};

	template<class T>
	struct CallResponse {
		boost::optional<nvimRpc::packer::Error> error;
		boost::optional<T> value;
	};

	class CallInterface {
		public:
			virtual void fulfillPromise(const std::vector<char>& rawResponse) = 0;
			virtual CallState state() = 0;
	};

	template<class T, class... U> class Call: public CallInterface {
		private: 
			CallState _state;
			const char* _data;
			size_t _dataSize;
			std::promise<CallResponse<T>> _promise;

		public:
			Call<T, U...>(const nvimRpc::packer::PackedRequest<U...>* request) {
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

			std::future<CallResponse<T>> getFuture() {
				return _promise.get_future();
			}

			void fulfillPromise(const std::vector<char>& rawResponse) {
				nvimRpc::packer::PackedRequestResponse<T> unpackedResponse(rawResponse);
				// TODO check how to handle error if unpackedResponse.error() is not nil
				_promise.set_value({.error=unpackedResponse.error(), .value=unpackedResponse.value()});
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

		public:
			CallDispatcher(const Tcp::Connector* connector): _connector(connector) {
				_callMap = std::map<int, CallInterface*>();
				_callMap_mtx = new std::mutex();
				_connector_mtx = new std::mutex();
				_thread = NULL;
			}

			template <typename T, typename ...U>
			std::future<CallResponse<T>> placeCall(const nvimRpc::packer::PackedRequest<U...>* request) {
				std::lock_guard lockCallMap(*_callMap_mtx);
				std::lock_guard lockConnector(*_connector_mtx);
				Call<T, U...>* callToPlace = new Call<T, U...>(request);
				_callMap[request->id()] = callToPlace;
				_connector->send(callToPlace->data(), callToPlace->size());

				return callToPlace->getFuture();
			}

			void fulfillPlacedCall(int msgId, const std::vector<char>& rawResponse) {
				std::lock_guard lockCallMap(*_callMap_mtx);

				auto call = _callMap[msgId];
				if (call->state() == DONE) {
					throw std::runtime_error(std::string("request with id ") + std::to_string(msgId) + std::string(" already fulfilled"));
				}

				call->fulfillPromise(rawResponse);
				// remove call from map?
			}

			void listenToConnector() {
				nvimRpc::packer::MessageIdentifier messageIdentifier;
				while (_isConnectorConnected()) {
					auto readFromSocket = _readFromSocket();

					if (readFromSocket.size() > 0) {
						auto messageIdentifier = nvimRpc::packer::MsgPacker::getMessageTypeAndId(readFromSocket);

						switch (messageIdentifier.type) {
							case nvimRpc::packer::MessageType::RESPONSE:
								messageIdentifier = nvimRpc::packer::MsgPacker::getMessageTypeAndId(readFromSocket);
								fulfillPlacedCall(messageIdentifier.id, readFromSocket);
								break;
							case nvimRpc::packer::MessageType::NOTIFY:
								//figure out what to do with notifications
								throw std::runtime_error("Notifications handling has not been implemented yet because I'm a lazy fuck");

						}
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
