#pragma once

#include <cstddef>
#include "lib.h"

namespace async {
//Multithread wrappers for async calls
using handle_t = size_t;
std::unordered_map<handle_t, Application&> contexts; //global variable for context swtiching
handle_t connect(std::size_t bulk);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

}
