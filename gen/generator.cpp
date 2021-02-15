#include <iostream>
# include <fstream>

# include "msgpack.hpp"
# include "./gen.hpp"

namespace generator {
	std::vector<BaseTypeMatch> typeMatches {
		{"Nil", "void"},
		{"Void", "void"},
		{"void", "void"},
		{"Boolean", "bool"},
		{"Integer", "int64_t"},
		{"Float", "double"},
		{"String", "std::string"},
		{"Dictionary", "nvimRpc::types::Dictionary"},
		{"Object", "msgpack::type::ext"},
		{"Array", "nvimRpc::types::Array"},
	};
	const ArrayMatch arrayMatch("^ArrayOf\\(([^\\),]+)(,[^\\)]+)?\\)$");

	void addApiTypes(const std::map<std::string, nvimApiMetadata::ApiMetaTypes>& apiTypes) {
		for (auto it: apiTypes) {
			typeMatches.push_back({it.first, "msgpack::type::ext"});
		}
	}

	std::string getType(const std::string& nvimType) {
		std::smatch sm;
		if (std::regex_match(nvimType, sm, std::regex(arrayMatch.nvimTypeName))) {
			return arrayMatch.getCppType(getType(sm[1].str()));
		}

		for (auto it: typeMatches) {
			if (it.nvimTypeName == nvimType) {
				return it.cppType;
			}
		}

		// TODO : at some point I got faced with a "LuaRef" type I was not finding in the api doc, so I just went with that solution, but it should be investigated
		return "msgpack::type::ext";
	}

	std::vector<ApiFunctionParam> getApiFunctionParams(const std::vector<nvimApiMetadata::ApiMetaParams>& params) {
		std::vector<ApiFunctionParam> ret;
		std::transform(params.begin(), params.end(), std::back_inserter(ret), [](nvimApiMetadata::ApiMetaParams p) { return ApiFunctionParam(p); });

		return ret;
	}

	void defineApiFunction(std::stringstream& client, nvimApiMetadata::ApiMetaFunction& apiFunction) {
			std::string fnType = getType(apiFunction.return_type);
			std::vector<ApiFunctionParam> fnParams = getApiFunctionParams(apiFunction.parameters);
			client
				<< fnType << " "
				<< apiFunction.name << "(";

			for (auto param = fnParams.begin(); param != fnParams.end(); param++) {
				std::string cppParamType = getType(param->type);
				if (param != fnParams.begin()) {
					client << ", ";
				}
				client << cppParamType << " " << param->name;
			}

			client << ") {}" << std::endl;
	}

	std::string generateNvimClient(const nvimApiMetadata::ApiMetaInfo& apiInfo) {
		std::stringstream client;

		client << DEFAULT_REQUIRES;

		addApiTypes(apiInfo.types);
		for (auto fn: apiInfo.functions) {
			defineApiFunction(client, fn);
		}

		std::cout << client.str();

		return client.str();
	} 
}
