function headerSetup(headerFile) {
    headerFile.write(`
        #ifndef NVIM_CLIENT
        #define NVIM_CLIENT
        # include <string>
        # include <utility>
        # include <iostream>
        # include <exception>

        # include "msgpack.hpp"
        # include "impl/MsgPacker.hpp"
        # include "impl/TcpConnector.hpp"
        # include "impl/types.hpp"

        namespace nvimRpc {
                struct ClientConfig {
                        std::string host;
                        int port;
                };

                class ClientError: std::exception {
                        private:
                                std::string _errorMessage;

                        public:
                                ClientError(const packer::Error& error) {
                                        _errorMessage = std::string(error.get<1>());
                                };

                                const char* what() const noexcept {
                                        return _errorMessage.data();
                                };
                };

                class Client {
                        private:
                                Tcp::Connector *_connector;
                                uint64_t _msgid;

                                template<typename T, typename...U>
                                        packer::PackedRequestResponse<T> _call(const std::string& method, const U&...args) {
                                                packer::PackedRequest<U...> packedRequest(method, _msgid++, args...);

                                                _connector->send(packedRequest.data(), packedRequest.size());
                                                std::vector<char> rawApiRes = _connector->read();

                                                return packer::PackedRequestResponse<T>(rawApiRes);
                                        };

                                        template<typename T>
                                                void _handleResponse(const packer::PackedRequestResponse<T>& response, T& ret) {
                                                        if (response.error()) {
                                                                throw ClientError(*(response.error()));
                                                        }
                                                        ret = *(response.value());
                                                }

                                        template<typename T>
                                                void _handleResponse(const packer::PackedRequestResponse<T>& response) {
                                                        if (response.error()) {
                                                                throw ClientError(*(response.error()));
                                                        }
                                                }


                        public:
                                Client(Tcp::Connector* connector) {
                                        this->_connector = connector;
                                        this->_msgid = 0;
                                };

                                void connect() {
                                        _connector->connect();
                                };


    `);
}

function headerConclude(headerFile) {
    headerFile.write(`
};
}

#endif
`);
}

module.exports = {
    headerSetup,
    headerConclude,
};
