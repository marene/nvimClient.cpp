#ifndef GEN_HPP
# define GEN_HPP

# include <iostream>
# include <regex>

# include "msgpack.hpp"

# define DEFINE_NVIMRPC_HEADERS R"define(#ifndef NVIM_CLIENT
# define NVIM_CLIENT

)define"

# define CLOSE_NVIMRPC_HEADERS R"close(
#endif /*NVIM_CLIENT*/
)close"

# define DEFAULT_REQUIRES R"includes(
# include <string>
# include <utility>
# include <iostream>
# include <exception>

# include "msgpack.hpp"
# include "impl/MsgPacker.hpp"
# include "impl/TcpConnector.hpp"
# include "impl/types.hpp"
)includes"

# define NVIMRPC_SETUP R"nvimrpcsetup(
namespace nvimRpc {
	struct ClientConfig {
		std::string host;
		int port;
	};

	class ClientError: std::exception {
		private:
			std::string _errorMessage;

		public:
			ClientError(const packer::Error& error) {
				_errorMessage = std::string(error.get<1>());
			};

			const char* what() const noexcept {
				return _errorMessage.data();
			};
	};

)nvimrpcsetup"

# define CLIENT_SETUP R"clientsetup(
	class Client {
		private:
			Tcp::Connector *_connector;
			uint64_t _msgid;

			template<typename T, typename...U>
				packer::PackedRequestResponse<T> _call(const std::string& method, const U&...args) {
					packer::PackedRequest<U...> packedRequest(method, _msgid++, args...);

					_connector->send(packedRequest.data(), packedRequest.size());
					std::vector<char> rawApiRes = _connector->read();

					return packer::PackedRequestResponse<T>(rawApiRes);
				};

				template<typename T>
					void _handleResponse(const packer::PackedRequestResponse<T>& response, T& ret) {
						if (response.error()) {
							throw ClientError(*(response.error()));
						}
						ret = *(response.value());
					}

				template<typename T>
					void _handleResponse(const packer::PackedRequestResponse<T>& response) {
						if (response.error()) {
							throw ClientError(*(response.error()));
						}
					}


		public:
			Client(Tcp::Connector* connector) {
				this->_connector = connector;
				this->_msgid = 0;
			};

			void connect() {
				_connector->connect();
			};

)clientsetup"

# define CLOSE_CLIENT "\n};"
# define CLOSE_NVIMRPC "\n}"

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

		ApiFunctionParam(std::string type, std::string name): type(type), name(name) {}
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
