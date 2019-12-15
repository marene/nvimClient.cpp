#ifndef MSG_PACKER
# define MSG_PACKER
# define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
# define MSGPACK_USE_BOOST


# include <iostream>
# include <boost/variant/get.hpp>

# include "msgpack.hpp"

namespace nvimRpc {
	namespace packer {
		using Packer = msgpack::packer<msgpack::sbuffer>;
		using Object = msgpack::object;
		using Void = msgpack::type::nil_t;
		enum {
			REQUEST  = 0,
			RESPONSE = 1,
			NOTIFY   = 2
		};

		template<class T>
			Packer& pack(Packer& pk, const T& t) {
				return pk << t;
			}

		Packer& pack(Packer& pack) {
			return pack;
		}

		template <typename...T>
			class PackedRequest {
				private:
					msgpack::sbuffer _buffer;
					Packer* _packer;
				public:
					PackedRequest(const std::string& method, uint64_t msgid, const T&...args) {
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

					char* data() {
						return _buffer.data();
					};
					size_t size() {
						return _buffer.size();
					};
			};

		template <typename T>
			class PackedRequestResponse {
				private:
					msgpack::object_handle *_objectHandle;
					std::vector<char> _rawResponse;

				public:
					PackedRequestResponse(const std::vector<char>& rawResponse) {
						_rawResponse = std::vector<char>(rawResponse);
					};

					T value() {
						msgpack::object_handle objectHandle = msgpack::unpack(_rawResponse.data(), _rawResponse.size());
						msgpack::type::tuple<uint64_t, uint64_t, Object, Object> unpackedResponse;
						objectHandle.get().convert(unpackedResponse);
						Object objectValue = unpackedResponse.get<3>();

						return objectValue.as<T>();
					}
			};

		class MsgPacker {
			public:

		};
	}
}

#endif /* !MSG_PACKER */
