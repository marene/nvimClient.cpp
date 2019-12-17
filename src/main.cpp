#include <iostream>
#include <unistd.h>
#include <iostream>

#include "nvimClient.hpp"

int main(int argc, char **argv) {
	nvimRpc::Client *client = new nvimRpc::Client(
		new Tcp::Connector({"127.0.0.1", 6666})
	);

	client->connect();
	try {
		auto buffers = client->listBufs();
	} catch (nvimRpc::ClientError& e) {
		std::cout << "Client error caught: " << e.what() << std::endl;
	}
	return 0;
}
