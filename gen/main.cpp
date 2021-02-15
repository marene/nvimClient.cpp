#include "gen.hpp"

int main() {
	nvimApiMetadata::ApiMetaInfo apiInfo = nvimApiMetadata::generate("/tmp/apiinfo");

	for (auto& type : apiInfo.types) {
		std::cout << type.first << " : " << type.second.prefix << std::endl;
	}

	/*
	 *for (auto& fn : apiInfo.functions) {
	 *  std::cout << fn.name << " : " << fn.return_type << std::endl;
	 *}
	 */

	try {
		generator::generateNvimClient(apiInfo);
	} catch (std::exception& e) {
		std::cout << "error ----------->" << e.what() << std::endl;
	}

	return 0;
}
