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
		std::transform(params.begin(), params.end(), std::back_inserter(ret), [](nvimApiMetadata::ApiMetaParams p) {
			return ApiFunctionParam(getType(p[0]), p[1]);
		});

		return ret;
	}

	std::string getApiFunctionPrototype(std::string fnType, std::string fnName, std::vector<ApiFunctionParam> fnParams) {
		std::string fnPrototype = "";

		fnPrototype += fnType + " " + fnName + "(";
		for (auto param = fnParams.begin(); param != fnParams.end(); param++) {
			if (param != fnParams.begin()) {
				fnPrototype += ", ";
			}
			fnPrototype += param->type + " " + param->name;
		}

		fnPrototype += ") {\n";

		return fnPrototype;
	}

	std::string getVoidApiFunctionImplementation(std::string fnName, std::vector<ApiFunctionParam> fnParams) {
		std::string impl = "\tauto packedResponse = _call<packer::Void>(\"" + fnName + "\"";

		for (auto param = fnParams.begin(); param != fnParams.end(); param++) {
			impl += ", " + param->name;
		}

		impl += ");\n\n\t_handleResponse(packedResponse);\n}";

		return impl;
	}

	std::string getTypedApiFunctionImplementation(std::string fnType, std::string fnName, std::vector<ApiFunctionParam> fnParams) {
		std::string impl =
			"\t" + fnType + " ret;\n"
			+ "\tauto packedResponse = _call<" + fnType + ">"
			+ "(\"" + fnName + "\"";

		for (auto param = fnParams.begin(); param != fnParams.end(); param++) {
			impl += ", " + param->name;
		}

		impl += ");\n\n\t_handleResponse(packedResponse, ret);\n\treturn ret;\n}";

		return impl;
	}

	std::string getApiFunctionImplementation(std::string fnType, std::string fnName, std::vector<ApiFunctionParam> fnParams) {
		if (fnType == "void") {
			return getVoidApiFunctionImplementation(fnName, fnParams);
		}

		return getTypedApiFunctionImplementation(fnType, fnName, fnParams);
	}

	void defineApiFunction(std::stringstream& client, nvimApiMetadata::ApiMetaFunction& apiFunction) {
			std::string fnType = getType(apiFunction.return_type);
			std::vector<ApiFunctionParam> fnParams = getApiFunctionParams(apiFunction.parameters);

			client
				<< getApiFunctionPrototype(fnType, apiFunction.name, fnParams)
				<< getApiFunctionImplementation(fnType, apiFunction.name, fnParams)
				<< std::endl << std::endl;
	}

	std::string generateNvimClient(const nvimApiMetadata::ApiMetaInfo& apiInfo) {
		std::stringstream client;

		client
			<< DEFINE_NVIMRPC_HEADERS
			<< DEFAULT_REQUIRES
			<< NVIMRPC_SETUP
			<< CLIENT_SETUP
			<< std::endl << std::endl;

		addApiTypes(apiInfo.types);
		for (auto fn: apiInfo.functions) {
			defineApiFunction(client, fn);
		}

		client
			<< CLOSE_CLIENT
			<< CLOSE_NVIMRPC
			<< CLOSE_NVIMRPC_HEADERS;

		std::cout << client.str();

		return client.str();
	} 
}
