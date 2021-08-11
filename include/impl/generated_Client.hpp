
#ifndef NVIM_CLIENT
#define NVIM_CLIENT
#include <exception>
#include <iostream>
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

    template <typename T, typename... U>
    packer::PackedRequestResponse<T> _call(const std::string& method, const U&... args) {
        packer::PackedRequest<U...> packedRequest(method, _msgid++, args...);

        _connector->send(packedRequest.data(), packedRequest.size());
        std::vector<char> rawApiRes = _connector->read();

        return packer::PackedRequestResponse<T>(rawApiRes);
    };

		template<typename... U>
		packer::PackedRequest<U...>* _packRequest(const std::string& method, const U&... args) {
			//TODO use smart pointers
			return new packer::PackedRequest<U...>(method, _msgid++, args...);
		}

    template <typename T> void _handleResponse(const packer::PackedRequestResponse<T>& response, T& ret) {
        if (response.error()) {
            throw ClientError(*(response.error()));
        }
        ret = *(response.value());
    }

    template <typename T> void _handleResponse(const packer::PackedRequestResponse<T>& response) {
        if (response.error()) {
            throw ClientError(*(response.error()));
        }
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
			// TODO maybe connection of the connector should be the responsability of the dispatcher, so it can wait for all pending request to be fulfilled
			_connector->disconnect();
			_dispatcherThread.join();
		}

		std::future<dispatcher::CallResponse<packer::Void>> vim_command(std::string command) {
			auto packedRequest = _packRequest("vim_command", command);

			return _dispatcher->placeCall<packer::Void, std::string>(packedRequest);
    }

		std::future<dispatcher::CallResponse<nvimRpc::types::Dictionary>> nvim_get_hl_by_name(std::string name, bool rgb) {
        nvimRpc::types::Dictionary ret;
				auto packedRequest = _packRequest("nvim_get_hl_by_name", name, rgb);

				return _dispatcher->placeCall<types::Dictionary, std::string, bool>(packedRequest);
    }
};
} // namespace nvimRpc

#endif
