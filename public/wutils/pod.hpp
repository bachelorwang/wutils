#pragma once
#include <type_traits>

namespace wutils {

template <typename T>
inline auto zero_initialize_pod(T& pod) ->
    typename std::enable_if<std::is_trivial_v<T>>::type {
  memset(&pod, 0, sizeof(T));
}

}  // namespace wutils
