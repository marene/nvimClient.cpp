#include <iostream>
#include <vector>

#include "Client.hpp"

using namespace nvimRpc;

Client::Client(const ClientConfig& config) {
	this->_connector = new Tcp::Connector(config.host, config.port);
	this->_msgid = 0;
}

void Client::connect() {
	_connector->connect();
}


template<class T>
Packer& Client::pack(Packer& pk, const T& t) {
	return pk << t;
}

Packer& Client::pack(Packer& pack) {
	return pack;
}

Object Client::deserialize(const std::vector<char>& raw) {
	msgpack::object_handle objectHandle = msgpack::unpack(raw.data(), raw.size());
	msgpack::object deserialized = objectHandle.get();

	return deserialized;
}

template<typename...U>
void Client::_call(const std::string& method, const U&...args) {
	msgpack::sbuffer buffer;
	Packer packer(&buffer);
	
	packer.pack_array(4) << (uint64_t)REQUEST
	                     << _msgid++
	                     << method;

	packer.pack_array(sizeof...(args));

	Client::pack(packer, args...);

	_connector->send(buffer.data(), buffer.size());
}
//[1,0,null,"foobar"]
std::string Client::getCurrentLine() {
	_call("nvim_get_current_line");

	std::vector<char> rawApiRes = _connector->read();

	Object deserialized = Client::deserialize(rawApiRes);
	std::cout << "Deserialized response: " << deserialized << std::endl;

	return "FooBar";
}

std::string Client::setCurrentLine(const std::string& line) {
	_call("nvim_set_current_line", line);
	std::vector<char> rawApiRes = _connector->read();

	Object deserialized = Client::deserialize(rawApiRes);
	std::cout << "Deserialized response: " << deserialized << std::endl;

	return "FooBar";
}
