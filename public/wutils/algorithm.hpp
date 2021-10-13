#pragma once
#include <cstddef>

namespace wutils {

size_t max(size_t lhs) {
  return lhs;
}

template <typename... Ts>
size_t max(size_t lhs, size_t rhs, Ts... rest) {
  return (::wutils::max)(lhs > rhs ? lhs : rhs, rest...);
}

}  // namespace wutils
