#include <iostream>
#include <unistd.h>
#include <iostream>

#include "TcpConnector.hpp"
#include "Client.hpp"

int main(int argc, char **argv) {
	nvimRpc::Client *client = new nvimRpc::Client(
		new Tcp::Connector({"127.0.0.1", 6666})
	);

	client->connect();
	client->setCurrentLine("FROM NVIM CLIENT");
	client->getCurrentLine();
	return 0;
}
