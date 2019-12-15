#ifndef TCP_CONNECTOR
# define TCP_CONNECTOR

# include <boost/asio.hpp>
# include <boost/array.hpp>
# include <vector>

namespace Tcp {
	class Connector {
		private:
			boost::asio::io_service *_io;
			boost::asio::ip::tcp::endpoint *_endpoint;
			boost::asio::ip::tcp::socket *_socket;
			bool _isConnected;


		public:
			Connector(const std::string &host, const int &port) {
				auto hostAddress = boost::asio::ip::address::from_string(host);

				_io = new boost::asio::io_service;
				_endpoint = new boost::asio::ip::tcp::endpoint(hostAddress, port);
				_socket = new boost::asio::ip::tcp::socket(*_io);
				_isConnected = false;
			};

			~Connector() {
				disconnect();
				free(_io);
				free(_endpoint);
				free(_socket);
			};

			void send(const char* buff, size_t size) {
				boost::system::error_code error;

				if (!_isConnected) {
					throw std::runtime_error("Attempting to write to disconnected socket");
				}

				_socket->write_some(boost::asio::buffer(buff, size), error);
			};

			std::vector<char> read() {
				std::array<char, 2048> buf;

				size_t sizeRead = _socket->read_some(boost::asio::buffer(buf));

				return std::vector<char>(buf.begin(), buf.begin() + sizeRead);
			};

			void connect() {
				boost::asio::socket_base::keep_alive option(true);

				_socket->connect(*_endpoint);
				_socket->set_option(option);
				_isConnected = true;
			};

			void disconnect() {
				_socket->close();
				_isConnected = false;
			};
	};
}

#endif /* !TCP_CONNECTOR */
