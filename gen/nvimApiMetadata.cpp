#include <iostream>
# include <fstream>

# include "msgpack.hpp"
# include "./gen.hpp"

namespace nvimApiMetadata {
	ApiMetaInfo getApiMetaInfo(std::ifstream& metaInfoInput) {
		ApiMetaInfo metaInfo;
		std::vector<char> buffer(std::istreambuf_iterator<char>(metaInfoInput), {});

		auto objectHandle = msgpack::unpack(buffer.data(), buffer.size());
		auto unpacked = objectHandle.get();

		unpacked.convert(metaInfo);

		return metaInfo;
	}

	ApiMetaInfo generate(const std::string& apiInfoPath) {
		std::ifstream metaInfoInput(apiInfoPath, std::ios::binary);
		auto apiInfo = getApiMetaInfo(metaInfoInput);	
		return apiInfo;
	}
}
