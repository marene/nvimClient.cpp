#include <iostream>

#include "TcpConnector.hpp"

using namespace Tcp;

void print() {
	std::cout << "foo" << std::endl;
}

Connector::Connector(const std::string &host, const int &port) {
	auto hostAddress = boost::asio::ip::address::from_string(host);

	_io = new boost::asio::io_service;
	_endpoint = new boost::asio::ip::tcp::endpoint(hostAddress, port);
	_socket = new boost::asio::ip::tcp::socket(*_io);
	_isConnected = false;
}

Connector::~Connector() {
	disconnect();
	free(_io);
	free(_endpoint);
	free(_socket);
}

void Connector::connect() {
	boost::asio::socket_base::keep_alive option(true);

	_socket->connect(*_endpoint);
	_socket->set_option(option);
	_isConnected = true;
}

void Connector::disconnect() {
	_socket->close();
	_isConnected = false;
}

void Connector::send(const char* buff, size_t size) {
	std::cout << "Attempting to send [" << buff << "] through socket..." << std::endl;
	boost::system::error_code error;
	//boost::array<char, 128> dataArray;

	if (!_isConnected) {
		throw std::runtime_error("Attempting to write to disconnected socket");
	}

	//std::copy(data.begin(), data.end(), dataArray.begin());
	_socket->write_some(boost::asio::buffer(buff, size), error);
	std::cout << "Finished sending data" << std::endl;
}

std::vector<char> Connector::read() {
	std::array<char, 2048> buf;

	std::cout << "Reading from socket..." << std::endl;
	size_t sizeRead = _socket->read_some(boost::asio::buffer(buf));
	std::cout << "Finished reading from socket..." << std::endl;

	return std::vector<char>(buf.begin(), buf.begin() + sizeRead);
}

void Connector::awaitSync(int numberOfSeconds) {
	boost::asio::deadline_timer t(*_io);
	boost::asio::deadline_timer t1(*_io);

	t.expires_from_now(boost::posix_time::seconds(numberOfSeconds));
	t.wait();
	std::cout << "await 1 over" << std::endl;
	t1.expires_from_now(boost::posix_time::seconds(numberOfSeconds));
	t1.wait();
}

void Connector::awaitAsync(int numberOfSeconds) {
	boost::asio::deadline_timer t(*_io);
	boost::asio::deadline_timer t1(*_io);

	t.expires_from_now(boost::posix_time::seconds(numberOfSeconds));
	t.async_wait([](const boost::system::error_code& err) {
			std::cout << "Await over!" << std::endl;
			});
	std::cout << "should be async" << std::endl;
	t1.expires_from_now(boost::posix_time::seconds(numberOfSeconds));
	t1.async_wait([](const boost::system::error_code& err) {
			std::cout << "Await  2 over!" << std::endl;
			});
	_io->run();
}
