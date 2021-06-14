#ifndef NVIM_CLIENT_THREADED
# define NVIM_CLIENT_THREADED


# include <string>
# include <utility>
# include <iostream>
# include <exception>

# include "msgpack.hpp"
# include "impl/MsgPacker.hpp"
# include "impl/TcpConnector.hpp"
# include "impl/types.hpp"

namespace nvimRpc {
	struct ClientThreadedConfig {
		std::string host;
		int port;
	};

	class ClientThreadedError: std::exception {
		private:
			std::string _errorMessage;

		public:
			ClientThreadedError(const packer::Error& error) {
				_errorMessage = std::string(error.get<1>());
			};

			const char* what() const noexcept {
				return _errorMessage.data();
			};
	};

	namespace ApiCall {
		using ApiCallId = uint64_t;

		enum ApiCallState {
			PLACED,
			PENDING,
			DONE
		};
			class ApiCall {
				private:
					ApiCallState _state;
					ApiCallId _id;
					const char* _buffer;
					size_t _bufferSize;

				public:
					ApiCall(const packer::PackedRequest<T>* packer, ApiCallId id):_id(id) {
						_state = ApiCallState::PLACED;
						_buffer = packer->data();
						_bufferSize = packer->size();
					}
			};
	}

	class ClientThreaded {
		private:
			Tcp::Connector *_connector;
			ApiCall::ApiCallId _msgid;

			template<typename...U>
				packer::PackedRequest<U...> _packRequest(const std::string method, const U&...args) {
					return packer::PackedRequest(method, _msgid++, args...);
				}

			template<typename T, typename...U>
				packer::PackedRequestResponse<T> _call(const std::string& method, const U&...args) {
					auto packedRequest = _packRequest(method, args...);

					_connector->send(packedRequest.data(), packedRequest.size());
					std::vector<char> rawApiRes = _connector->read();

					return packer::PackedRequestResponse<T>(rawApiRes);
				};

			template<typename T>
				void _handleResponse(const packer::PackedRequestResponse<T>& response, T& ret) {
					if (response.error()) {
						throw ClientThreadedError(*(response.error()));
					}
					ret = *(response.value());
				}

			template<typename T>
				void _handleResponse(const packer::PackedRequestResponse<T>& response) {
					if (response.error()) {
						throw ClientThreadedError(*(response.error()));
					}
				}


		public:
			ClientThreaded(Tcp::Connector* connector) {
				this->_connector = connector;
				this->_msgid = 0;
			};

			void connect() {
				_connector->connect();
			};

			nvimRpc::types::Dictionary nvim_get_hl_by_name(std::string name, bool rgb) {
				nvimRpc::types::Dictionary ret;
				auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_hl_by_name", name, rgb);

				_handleResponse(packedResponse, ret);
				return ret;
			}

			void vim_command(std::string command) {
				auto packedResponse = _call<packer::Void>("vim_command", command);

				_handleResponse(packedResponse);
			}

			void threaded_vim_command(std::string command) {
				auto packedRequest = _packRequest("vim_command", command);
			}
	};
}
#endif /*NVIM_CLIENT*/
