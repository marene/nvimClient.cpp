#ifndef MSG_PACKER
# define MSG_PACKER
# define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
# define MSGPACK_USE_BOOST

# include <iostream>
# include <map>
# include <experimental/optional>

# include "msgpack.hpp"

namespace nvimRpc {
	namespace packer {
		using Packer = msgpack::packer<msgpack::sbuffer>;
		using Object = msgpack::object;
		using Void = msgpack::type::nil_t;
		using Error = msgpack::type::tuple<uint64_t, std::string>;
		using HlGroupMap = struct {
			bool bold;
			uint64_t foreground;
			MSGPACK_DEFINE_MAP(bold, foreground);
		};
		enum {
			REQUEST  = 0,
			RESPONSE = 1,
			NOTIFY   = 2
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

						objectValue.convert_if_not_nil(_value);
						objectError.convert_if_not_nil(_error);
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

		};
	}
}

#endif /* !MSG_PACKER */
