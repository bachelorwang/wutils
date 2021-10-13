#pragma once
#include <cstddef>
#include <tuple>
#include <utility>
#include <wutils/algorithm.hpp>

namespace wutils {

namespace detail {

template <size_t Size>
using index_to_size = size_t;

static constexpr size_t align(size_t n, size_t m) {
  return (n + m - 1) & ~(m - 1);
}

template <typename TElements, typename TSizeIndexes, typename TOffsetIndexes>
struct layout_impl;

template <typename... TElements,
          size_t... TSizeIndexes,
          size_t... TOffsetIndexes>
struct layout_impl<std::tuple<TElements...>,
                   std::index_sequence<TSizeIndexes...>,
                   std::index_sequence<TOffsetIndexes...>> {
  enum {
    NumTypes = sizeof...(TElements),
    NumSizes = sizeof...(TSizeIndexes),
    NumOffsets = sizeof...(TOffsetIndexes)
  };

  static_assert(NumTypes > 0);

  constexpr explicit layout_impl(index_to_size<TSizeIndexes>... sizes)
      : sizes_{sizes...} {}

  using element_types = std::tuple<TElements...>;

  template <size_t TIndex>
  using element_type = std::tuple_element_t<TIndex, element_types>;

  template <size_t TIndex>
  using element_alignment = std::alignment_of<element_type<TIndex>>;

  static constexpr size_t alignment() {
    return ::wutils::max(element_alignment<TSizeIndexes>::value...);
  }

  template <size_t TIndex, std::enable_if_t<TIndex == 0, int> = 0>
  constexpr size_t offset() const {
    return 0;
  }

  template <size_t TIndex, std::enable_if_t<TIndex != 0, int> = 0>
  constexpr size_t offset() const {
    return align(offset<TIndex - 1>() +
                     sizeof(element_type<TIndex - 1>) * sizes_[TIndex - 1],
                 element_alignment<TIndex>::value);
  }

  constexpr size_t alloc_size() const {
    return offset<NumTypes - 1>() +
           sizeof(element_type<NumTypes - 1>) * sizes_[NumTypes - 1];
  }

  template <size_t TIndex>
  element_type<TIndex>* pointer(void* p) {
    auto pb = reinterpret_cast<char*>(p);
    return reinterpret_cast<element_type<TIndex>*>(pb + offset<TIndex>());
  }

 private:
  size_t sizes_[sizeof...(TElements)];
};

}  // namespace detail

template <typename... Ts>
using layout = detail::layout_impl<std::tuple<Ts...>,
                                   std::make_index_sequence<sizeof...(Ts)>,
                                   std::make_index_sequence<sizeof...(Ts)>>;

}  // namespace wutils
