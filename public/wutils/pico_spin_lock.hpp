#pragma once
#include <atomic>
#include <chrono>
#include <thread>
#include <type_traits>
#include "wutils/debugging.hpp"

namespace wutils {

namespace detail {

class sleeper {
  uint32_t spin_count;

  static constexpr uint32_t max_spin = 4000;

 public:
  static constexpr std::chrono::nanoseconds min_yield_time =
      std::chrono::microseconds(500);

  constexpr sleeper() noexcept : spin_count(0) {}

  void wait() noexcept {
    if (spin_count < max_spin) {
      ++spin_count;
      asm_volatile_pause();
    } else {
      std::this_thread::sleep_for(min_yield_time);
    }
  }
};

}  // namespace detail

template <typename TIntegerType, int TBit = sizeof(TIntegerType) * 8 - 1>
struct pico_spin_lock {
  static_assert(std::is_unsigned_v<TIntegerType>);
  static_assert(std::is_integral_v<TIntegerType>);
  static constexpr TIntegerType mask = TIntegerType(1) << TBit;

  void init(TIntegerType value = 0) {
    WUTILS_CHECK(!(value & mask));
    auto l = atomic_cast();
    l->store(value, std::memory_order_release);
  }

  TIntegerType get_data() const {
    return atomic_cast()->load(std::memory_order_relaxed) & ~mask;
  }

  void set_data(TIntegerType value) {
    WUTILS_CHECK(!(value & mask));
    auto l = atomic_cast();
    l->store((l->load(std::memory_order_relaxed) & mask) | value,
             std::memory_order_relaxed);
  }

  bool try_lock() const { return try_lock_internal(); }

  void lock() const {
    detail::sleeper sleeper;
    while (!try_lock_internal()) {
      sleeper.wait();
    }
  }

  void unlock() const {
    auto previous =
        atomic_cast()->fetch_and(~mask, std::memory_order_release);
  }

 private:
  mutable TIntegerType value_;

  inline std::atomic<TIntegerType>* atomic_cast() const {
    return reinterpret_cast<std::atomic<TIntegerType>*>(&value_);
  }

  bool try_lock_internal() const {
    auto previous =
        atomic_cast()->fetch_or(mask, std::memory_order_acquire) & mask;
    return !previous;
  }
};

}  // namespace wutils
