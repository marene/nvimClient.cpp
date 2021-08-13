#ifndef MSG_PACKER
# define MSG_PACKER
# define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
# define MSGPACK_USE_BOOST

# include <iostream>
# include <map>

# include "msgpack.hpp"

namespace nvimRpc {
	namespace packer {
		using Packer = msgpack::packer<msgpack::sbuffer>;
		using Object = msgpack::object;
		using Void = msgpack::type::nil_t;
		using Error = msgpack::type::tuple<uint64_t, std::string>;
		using MessageType = enum {
			REQUEST  = 0,
			RESPONSE = 1,
			NOTIFY   = 2
		};

		using MessageIdentifier = struct messageId_s {
			int id;
			int type;
		};

		template<class T>
			Packer& pack(Packer& pk, const T& t) {
				return pk << t;
			}

		template<typename...T>
			Packer& pack(Packer& pk, const T&...args) {
				(pk << ... << args);
				return pk;
			}

		Packer& pack(Packer& pack) {
			return pack;
		}

		template <typename...T>
			class PackedRequest {
				private:
					msgpack::sbuffer _buffer;
					Packer* _packer;
					uint64_t _id;

				public:
					PackedRequest(const std::string& method, uint64_t msgid, const T&...args): _id(msgid) {
						_buffer = msgpack::sbuffer();
						_packer = new Packer(&_buffer);

						_packer->pack_array(4) << (uint64_t)REQUEST
							<< msgid
							<< method;

						_packer->pack_array(sizeof...(args));

						pack(*_packer, args...);
					};

					~PackedRequest() {
						free(_packer);
					};

					const char* data() const {
						return _buffer.data();
					};

					const Packer* getPacker() const {
						return _packer;
					}

					size_t size() const {
						return _buffer.size();
					};

					uint64_t id() const {
						return _id;
					}
			};

		class PackedRequestResponse {
			private:
				std::vector<char> _rawResponse;
				uint64_t _msgType;
				uint64_t _msgId;
				Object _objectValue;
				Object _objectError;

			public:
				PackedRequestResponse() {};
				PackedRequestResponse(const std::vector<char>& rawResponse, size_t& offset) {
					_rawResponse = std::vector<char>(rawResponse);

					msgpack::object_handle objectHandle = msgpack::unpack(_rawResponse.data(), _rawResponse.size(), offset);
					msgpack::type::tuple<uint64_t, uint64_t, Object, Object> unpackedResponse;
					objectHandle.get().convert(unpackedResponse);
					_msgType = unpackedResponse.get<0>();
					_msgId = unpackedResponse.get<1>();
					_objectValue = unpackedResponse.get<3>();
					_objectError = unpackedResponse.get<2>();
				};


				template<class T>
				bool value(T& value) const {
					return _objectValue.convert_if_not_nil(value);
				}

				bool error(packer::Error& error) const {
					return _objectError.convert_if_not_nil(error);
				}

				uint64_t type() const {
					return _msgType;
				}

				uint64_t id() const {
					return _msgId;
				}
		};
	}
}

#endif /* !MSG_PACKER */
