# include "msgpack.hpp"

namespace nvimRpc {
	namespace packer {
		using Packer = msgpack::packer<msgpack::sbuffer>;
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

		template <typename...T>
			class PackedRequestResponse {
				private:
					msgpack::object _deserialized;

				public:
					PackedRequestResponse(const std::vector<char>& rawResponse) {
						msgpack::object_handle objectHandle = msgpack::unpack(rawResponse.data(), rawResponse.size());

						_deserialized = objectHandle.get();
					};

					const msgpack::object& deserialized() {
						return _deserialized;
					}
			};

		class MsgPacker {
			public:

		};
	}
}
