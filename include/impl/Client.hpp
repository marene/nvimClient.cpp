#ifndef NVIM_CLIENT
# define NVIM_CLIENT

# include <string>
# include <utility>
# include <iostream>
# include <exception>

# include "msgpack.hpp"

# include "impl/MsgPacker.hpp"
# include "impl/TcpConnector.hpp"
# include "impl/types.hpp"

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

			void command(const std::string& command) {
				auto packedResponse = _call<packer::Void>("nvim_command", command);

				_handleResponse(packedResponse);
			}

			packer::HlGroupMap getHlByName(const std::string& hlName, const bool& rgb) {
				packer::HlGroupMap ret;
				auto packedResponse = _call<packer::HlGroupMap>("nvim_get_hl_by_name", hlName, rgb);

				_handleResponse(packedResponse, ret);
				return ret;
			}

			packer::HlGroupMap getHlById(const uint64_t& hlId, const bool& rgb) {
				packer::HlGroupMap ret;
				auto packedResponse = _call<packer::HlGroupMap>("nvim_get_hl_by_id", hlId, rgb);

				_handleResponse(packedResponse, ret);
				return ret;
			}

			void feedkeys(const std::string& keys, const std::string& mode, bool escape_csi = false) {
				auto packedResponse = _call<packer::Void>("nvim_feedkeys", keys, mode, escape_csi);

				_handleResponse(packedResponse);
			};

			size_t input(const std::string& input) {
				size_t ret;
				auto packedResponse = _call<size_t>("nvim_input", input);

				_handleResponse(packedResponse, ret);
				return ret;
			}

			// TODO implement nvim_replace_termcodes

			std::string commandOutput(const std::string& command) {
				std::string ret;
				auto packedResponse = this->_call<std::string>("nvim_command_output", command);

				_handleResponse(packedResponse, ret);
				return ret;
			}

			template <typename T>
				T eval(const std::string& expression) {
					T ret;
					auto packedResponse = this->_call<T>("nvim_eval", expression);

					_handleResponse(packedResponse, ret);
					return ret;
				}

			// TODO implement nvim_execute_lua

			template <typename T, typename ...U>
				T callFunction(const std::string& functionName, const U& ...args) {
					T ret;
					packer::PackedRequestResponse<T> packedResponse;
					
					if (sizeof...(args) == 0) {
						packedResponse = this->_call<T>("nvim_call_function", functionName, std::vector<packer::Void>());
					} else {
						packedResponse = this->_call<T>("nvim_call_function", functionName, args...);
					}

					_handleResponse(packedResponse, ret);
					return ret;
				};

			// TODO implement nvim_call_dict_function

			size_t strwidth(const std::string& text) {
				size_t ret;
				auto packedResponse = this->_call<size_t>("nvim_strwidth", text);

				_handleResponse(packedResponse, ret);
				return ret;
			}

			std::vector<std::string> listRuntimePaths() {
				std::vector<std::string> ret;
				auto packedResponse = this->_call<std::vector<std::string>>("nvim_list_runtime_paths");

				_handleResponse(packedResponse, ret);
				return ret;
			}

			void setCurrentDir(const std::string& dir) {
				auto packedResponse = this->_call<packer::Void>("nvim_set_current_dir", dir);

				_handleResponse(packedResponse);
			}

			std::string getCurrentLine() {
				std::string ret;
				auto packedResponse = _call<std::string>("nvim_get_current_line");

				_handleResponse(packedResponse, ret);
				return ret;
			};

			void setCurrentLine(const std::string& line) {
				auto packedResponse = _call<packer::Void>("nvim_set_current_line", line);

				_handleResponse(packedResponse);
			};

			void delCurrentLine() {
				auto packedResponse = _call<packer::Void>("nvim_del_current_line");

				_handleResponse(packedResponse);
			}

			template <typename T>
				T getVar(const std::string& varName) {
					T ret;
					auto packedResponse = _call<T>("nvim_get_var", varName);

					_handleResponse(packedResponse, ret);
					return ret;
				}

			template <typename T>
				void setVar(const std::string& varName, T value) {
					auto packedResponse = _call<T>("nvim_set_var", varName, value);

					_handleResponse(packedResponse);
				}

			void delVar(const std::string& varName) {
				auto packedResponse = _call<packer::Void>("nvim_del_var", varName);

				_handleResponse(packedResponse);
			}

			template <typename T>
				T getVVar(const std::string& vVarName) {
					T ret;
					auto packedResponse = _call<T>("nvim_get_vvar", vVarName);

					_handleResponse(packedResponse, ret);
					return ret;
				}

			template <typename T>
				T getOption(const std::string& optionName) {
					T ret;
					auto packedResponse = _call<T>("nvim_get_option", optionName);

					_handleResponse(packedResponse, ret);
					return ret;
				}

			template <typename T>
				void setOption(const std::string& optionName, T optionValue) {
					auto packedResponse = _call<T>("nvim_set_option", optionName, optionValue);

					_handleResponse(packedResponse);
				}

			void outWrite(const std::string& message) {
				auto packedResponse = _call<std::string>("nvim_out_write", message);

				_handleResponse(packedResponse);
			}

			void errWrite(const std::string& errMessage) {
				auto packedResponse = _call<std::string>("nvim_err_write", errMessage);

				_handleResponse(packedResponse);
			}

			void errWriteLn(const std::string& errMessage) {
				std::string errMessageLn;
				auto packedResponse = _call<std::string>("nvim_err_write", errMessage + "\n");

				_handleResponse(packedResponse);
			}

			std::vector<nvimRpc::types::BufferHandle> listBufs() {
				std::vector<nvimRpc::types::BufferHandle> ret;

				auto packedRespone = _call<std::vector<nvimRpc::types::BufferHandle>>("nvim_list_bufs");

				_handleResponse(packedRespone, ret);
				return ret;
			}
	};

} 

#endif /* !NVIM_CLIENT */
