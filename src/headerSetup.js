function headerSetup() {
    return `
#ifndef NVIM_CLIENT
#define NVIM_CLIENT
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "impl/MsgPacker.hpp"
#include "impl/TcpConnector.hpp"
#include "impl/types.hpp"
#include "impl/CallDispatcher.hpp"
#include "msgpack.hpp"

namespace nvimRpc {
	struct ClientConfig {
		std::string host;
		int port;
	};

	class ClientError : std::exception {
		private:
			std::string _errorMessage;

		public:
			ClientError(const packer::Error& error) { _errorMessage = std::string(error.get<1>()); };

			const char* what() const noexcept { return _errorMessage.data(); };
	};

	class Client {
		private:
			Tcp::Connector* _connector;
			dispatcher::CallDispatcher* _dispatcher;
			std::thread _dispatcherThread;
			uint64_t _msgid;

			template<typename... U>
				std::shared_ptr<packer::PackedRequest<U...>> _packRequest(const std::string& method, const U&... args) {
					return std::shared_ptr<packer::PackedRequest<U...>>(new packer::PackedRequest<U...>(method, _msgid++, args...));
				}
		public:
			Client(Tcp::Connector* connector) {
				this->_connector = connector;
				this->_dispatcher = new dispatcher::CallDispatcher(connector);
				this->_msgid = 0;
			};

			void connect() {
				_connector->connect();
				_dispatcherThread = dispatcher::CallDispatcher::startCallDispatcher(_dispatcher);

			};

			void disconnect() {
				_connector->disconnect();
				_dispatcherThread.join();
			}

    `;
}

function headerConclude() {
    return `
};
}

#endif
`;
}

module.exports = {
    headerSetup,
    headerConclude,
};
