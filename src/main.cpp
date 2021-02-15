#include <unistd.h>
#include <iostream>

#include "nvimClient.hpp"

int main(int argc, char **argv) {
	nvimRpc::Client *client = new nvimRpc::Client(
		new Tcp::Connector({std::string("127.0.0.1"), 6666})
	);

	try {
	client->connect();
	} catch (std::exception& e) {
		std::cerr << "Failed to connect to nvim server: " << e.what() << std::endl;
	}

	try {
		auto buffers = client->listBufs();
		client->command(":highlight FooBar cterm=bold,underline, ctermfg=1, ctermbg=2");
		auto hlGroup = client->getHlByName("FooBar", false);
		for (auto it: hlGroup) {
			std::cout << it.first << std::endl;
		}
	} catch (nvimRpc::ClientError& e) {
		std::cout << "Client error caught: " << e.what() << std::endl;
	}
	return 0;
}
