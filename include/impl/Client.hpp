#ifndef NVIM_CLIENT
# define NVIM_CLIENT

# include <string>
# include <utility>
# include <iostream>

# include "msgpack.hpp"

# include "impl/MsgPacker.hpp"
# include "impl/TcpConnector.hpp"

namespace nvimRpc {
	struct ClientConfig {
		std::string host;
		int port;
	};

	class Client {
		private:
			Tcp::Connector *_connector;
			uint64_t _msgid;

			template<typename...U>
				void _call(const std::string& method, const U&...args) {
					packer::PackedRequest<U...> packedRequest(method, _msgid++, args...);

					_connector->send(packedRequest.data(), packedRequest.size());
				};


		public:
			Client(Tcp::Connector* connector) {
				this->_connector = connector;
				this->_msgid = 0;
			};

			void connect() {
				_connector->connect();
			};

			std::string getCurrentLine() {
				_call("nvim_get_current_line");

				std::vector<char> rawApiRes = _connector->read();

				packer::PackedRequestResponse<std::string> packedResponse(rawApiRes);
				return packedResponse.value();
			};

			void setCurrentLine(const std::string& line) {
				_call("nvim_set_current_line", line);
				std::vector<char> rawApiRes = _connector->read();

				packer::PackedRequestResponse<packer::Void> packedResponse(rawApiRes);
			};
	};

} 

#endif /* !NVIM_CLIENT */
