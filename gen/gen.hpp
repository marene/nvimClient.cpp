#ifndef GEN_HPP
# define GEN_HPP

# include <iostream>
# include <regex>

# include "msgpack.hpp"

# define STRINGIFY(...) #__VA_ARGS__
# define DEFAULT_REQUIRES R"includes(
# include "msgpack.hpp"
# include "impl/MsgPacker.hpp"
# include "impl/TcpConnector.hpp"
# include "impl/types.hpp"
		)includes"

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

	ApiMetaInfo getApiMetaInfo(std::ifstream& metaInfoInput);
	ApiMetaInfo generate(const std::string& apiInfoPath);
}

namespace generator {
	struct BaseTypeMatch {
		std::string nvimTypeName;
		std::string cppType;
	};

	struct ApiFunctionParam {
		std::string type;
		std::string name;

		ApiFunctionParam(nvimApiMetadata::ApiMetaParams& param) {
			this->type = param[0];
			this->name = param[1];
		}
	};

	class ContainerTypeMatch {
		public:
			std::string nvimTypeName;

			ContainerTypeMatch(const std::string& type): nvimTypeName(type) {};
			~ContainerTypeMatch() {};
			virtual std::string getCppType(const std::string& matchResults) const = 0;
	};

	class ArrayMatch: public ContainerTypeMatch {
		public:
			ArrayMatch(const std::string& type): ContainerTypeMatch(type) {};

			std::string getCppType(const std::string& matchResults) const {
				return "std::vector<" + matchResults + ">";
			};
	};

	std::string generateNvimClient(const nvimApiMetadata::ApiMetaInfo& apiInfo);
}

#endif /* !GEN_HPP */
