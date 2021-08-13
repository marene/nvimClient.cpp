#ifndef NVIM_CLIENT_TYPES
# define NVIM_CLIENT_TYPES

# include <cstdint>
# include "msgpack.hpp"

namespace nvimRpc {
	namespace types {
		using BufferHandle = msgpack::type::ext; // TODO implement more satisfactory/useful BufferHandle
		using Dictionary = std::map<std::string, msgpack::type::variant>;
		using Array = std::map<std::string, msgpack::type::variant>;
	}
}

#endif /* !NVIM_CLIENT_TYPES */
