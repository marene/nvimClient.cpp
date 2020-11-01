#include <iostream>
# include <fstream>

# include "msgpack.hpp"

namespace nvimApiMetadata {
	using ApiMetaVersion = struct {
		int major;
		int minor;
		int patch;
		int api_level;
		int api_compatible;
		bool api_prerelease;
		MSGPACK_DEFINE_MAP(major, minor, patch, api_level, api_compatible, api_prerelease);
	};

	using ApiMetaParams = std::vector<std::string>;

	using ApiMetaFunction = struct {
		bool method;
		std::string name;
		std::string return_type;
		std::vector<ApiMetaParams> parameters;
		int since;
		MSGPACK_DEFINE_MAP(method, name, return_type, parameters);
	};

	using ApiMetaTypes = struct {
		int id;
		std::string prefix;
		MSGPACK_DEFINE_MAP(id, prefix);
	};

	using ApiMetaUIEvents = struct {
		std::string name;
		std::vector<ApiMetaParams> parameters;
		int since;
		MSGPACK_DEFINE_MAP(name, parameters, since);
	};

	using ApiMetaId = struct {
		int id;
		MSGPACK_DEFINE_MAP(id);
	};
	using ApiMetaErrorType = std::map<std::string, ApiMetaId>;

	using ApiMetaInfo = struct {
		ApiMetaVersion version;
		std::vector<ApiMetaFunction> functions;
		std::vector<ApiMetaUIEvents> ui_events;
		std::vector<std::string> ui_options;
		ApiMetaErrorType error_types;
		std::map<std::string, ApiMetaTypes>  types;
		MSGPACK_DEFINE_MAP(version, functions, ui_events, ui_options, error_types, types);
	};


	ApiMetaInfo getApiMetaInfo(std::ifstream& metaInfoInput) {
		ApiMetaInfo metaInfo;
		std::vector<char> buffer(std::istreambuf_iterator<char>(metaInfoInput), {});

		auto objectHandle = msgpack::unpack(buffer.data(), buffer.size());
		auto unpacked = objectHandle.get();

		unpacked.convert(metaInfo);

		return metaInfo;
	}

	void generate(const std::string& apiInfoPath) {
		std::ifstream metaInfoInput(apiInfoPath, std::ios::binary);
		auto apiMetaInfo = getApiMetaInfo(metaInfoInput);	
	}
}

int main() {
	nvimApiMetadata::generate("/tmp/apiinfo");

	return 0;
}
