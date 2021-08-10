#include <thread>
#include <unistd.h>
#include <iostream>

#include "nvimClient.hpp"

void bar() {
	std::cout << "bar" << std::endl;
}

std::thread foo(dispatcher::CallDispatcher* dispatcher) {
	std::thread t(dispatcher::CallDispatcher::makeCallDistacherListen, dispatcher);

	return t;
}

int main(int argc, char **argv) {
	Tcp::Connector* connector = new Tcp::Connector({std::string("127.0.0.1"), 6666});
	nvimRpc::Client *client = new nvimRpc::Client(connector);
	dispatcher::CallDispatcher* dispatcher = new dispatcher::CallDispatcher(*connector);


	try {
		client->connect();
		auto t = dispatcher::CallDispatcher::startCallDispatcher(dispatcher);
		std::cout << "FOOBAR" << std::endl;
		t.join();
	} catch (std::exception& e) {
		std::cerr << "Failed to connect to nvim server: " << e.what() << std::endl;
	}

	try {
		client->vim_command(":highlight FooBar cterm=bold,underline, ctermfg=1, ctermbg=2");
		auto hlGroup = client->nvim_get_hl_by_name("FooBar", false);
		for (auto it: hlGroup) {
			std::cout << it.first << std::endl;
		}
	} catch (nvimRpc::ClientError& e) {
		std::cout << "Client error caught: " << e.what() << std::endl;
	}
	return 0;
}
