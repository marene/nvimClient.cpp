#include <future>
#include <thread>
#include <unistd.h>
#include <iostream>

#include "nvimClient.hpp"

int main(int argc, char **argv) {
	Tcp::Connector* connector = new Tcp::Connector({std::string("127.0.0.1"), 6666});
	nvimRpc::Client *client = new nvimRpc::Client(connector);
	dispatcher::CallDispatcher* dispatcher = new dispatcher::CallDispatcher(*connector);


	try {
		client->connect();
		auto t = dispatcher::CallDispatcher::startCallDispatcher(dispatcher);
		auto nvimCommandRequest = new nvimRpc::packer::PackedRequest<std::string>("nvim_command", 0, ":highlight FooBar cterm=bold,underline, ctermfg=1, ctermbg=2");
		auto getHlRequest = new nvimRpc::packer::PackedRequest<std::string, bool>("nvim_get_hl_by_name", 1, "FooBar", false);
		dispatcher->placeCall<nvimRpc::packer::Void, std::string>(nvimCommandRequest);
		auto fut = dispatcher->placeCall<nvimRpc::types::Dictionary, std::string, bool>(getHlRequest);
		std::cout << "FOOBAR" << std::endl;
		fut.wait();
		std::cout << "FOOBAR" << std::endl;
		auto hlGroup = fut.get().value.value();
		for (auto it: hlGroup) {
			std::cout << it.first << std::endl;
		}
		t.join();
	} catch (std::exception& e) {
		std::cerr << "Failed to connect to nvim server: " << e.what() << std::endl;
	}

 // try {
		//client->vim_command(":highlight FooBar cterm=bold,underline, ctermfg=1, ctermbg=2");
		//auto hlGroup = client->nvim_get_hl_by_name("FooBar", false);
		//for (auto it: hlGroup) {
			//std::cout << it.first << std::endl;
		//}
	//} catch (nvimRpc::ClientError& e) {
		//std::cout << "Client error caught: " << e.what() << std::endl;
	//}
	return 0;
}
