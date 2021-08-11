#include <future>
#include <thread>
#include <unistd.h>
#include <iostream>

#include "nvimClient.hpp"

int main(int argc, char **argv) {
	Tcp::Connector* connector = new Tcp::Connector({std::string("127.0.0.1"), 6666});
	nvimRpc::Client *client = new nvimRpc::Client(connector);


	try {
		client->connect();

		client->vim_command(":highlight FooBar cterm=bold,underline, ctermfg=1, ctermbg=2");

		auto fut = client->nvim_get_hl_by_name("FooBar", false);
		fut.wait();
		
		auto hlGroup = fut.get().value.value();
		for (auto it: hlGroup) {
			std::cout << it.first << std::endl;
		}
	} catch (std::exception& e) {
		std::cerr << "Failed to connect to nvim server: " << e.what() << std::endl;
	}

	return 0;
}
