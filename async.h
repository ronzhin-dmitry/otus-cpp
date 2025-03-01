#pragma once

#include <cstddef>
#include "lib.h"
#include <unordered_map>

namespace async {
//Multithread wrappers for async calls
using handle_t = size_t;
inline std::unordered_map<handle_t, std::unique_ptr<Application>> contexts; //global variable for context swtiching
handle_t connect(std::size_t bulk);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

}
