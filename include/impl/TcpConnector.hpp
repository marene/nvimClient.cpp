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
			Connector(const std::string &host, const int &port);
			~Connector();

			void send(const char* buff, size_t size);
			std::vector<char> read();
			void awaitSync(int numberOfSeconds);
			void awaitAsync(int numberOfSeconds);
			void connect();
			void disconnect();
	};
}

#endif /* !TCP_CONNECTOR */
