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

		template <typename T>
			class PackedRequestResponse {
				private:
					std::vector<char> _rawResponse;
					// TODO upgrade to std::optional
					boost::optional<packer::Error> _error;
					boost::optional<T> _value;

				public:
					PackedRequestResponse() {};
					PackedRequestResponse(const std::vector<char>& rawResponse) {
						_rawResponse = std::vector<char>(rawResponse);

						msgpack::object_handle objectHandle = msgpack::unpack(_rawResponse.data(), _rawResponse.size());
						msgpack::type::tuple<uint64_t, uint64_t, Object, Object> unpackedResponse;
						objectHandle.get().convert(unpackedResponse);
						Object objectValue = unpackedResponse.get<3>();
						Object objectError = unpackedResponse.get<2>();
						int type = unpackedResponse.get<0>();
						int msgId = unpackedResponse.get<1>();

						// [type, msgId, error, value]
						objectValue.convert_if_not_nil(_value);
						objectError.convert_if_not_nil(_error);

						std::cout << "type: " << type << " | msg id: " << msgId << std::endl;
					};

					const boost::optional<T>& value() const {
						return _value;
					}

					const boost::optional<packer::Error>& error() const {
						return _error;
					}
			};

		class MsgPacker {
			public:
				static MessageIdentifier getMessageTypeAndId(const std::vector<char>& rawMessage) {
					int id;
					int type;


					msgpack::object_handle objectHandle = msgpack::unpack(rawMessage.data(), rawMessage.size());
					msgpack::type::tuple<uint64_t, uint64_t, Object, Object> unpackedMessage;
					objectHandle.get().convert(unpackedMessage);
					type = unpackedMessage.get<0>();
					id = unpackedMessage.get<1>();

					return { .id = id, .type = type };
				}

		};
	}
}

#endif /* !MSG_PACKER */
