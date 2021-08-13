#ifndef NVIM_CLIENT_TYPES
#define NVIM_CLIENT_TYPES

#include "msgpack.hpp"
#include <cstdint>

namespace nvimRpc {
namespace types {
using BufferHandle = msgpack::type::ext; // TODO implement more satisfactory/useful BufferHandle
using Dictionary = std::map<std::string, msgpack::type::variant>;
using Array = std::map<std::string, msgpack::type::variant>;
} // namespace types
} // namespace nvimRpc

#endif /* !NVIM_CLIENT_TYPES */
