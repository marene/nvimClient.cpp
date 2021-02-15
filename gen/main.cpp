#include "gen.hpp"

int main() {
	nvimApiMetadata::ApiMetaInfo apiInfo = nvimApiMetadata::generate("/tmp/apiinfo");

	try {
		generator::generateNvimClient(apiInfo);
	} catch (std::exception& e) {
		std::cout <<  e.what() << std::endl;
	}

	return 0;
}
