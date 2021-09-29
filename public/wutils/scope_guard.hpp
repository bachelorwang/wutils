#pragma once
#include <iostream>

namespace wutils {

template <typename TFunctionType, bool TNoexcept>
class scope_guard final {
 public:
  scope_guard() = delete;
  scope_guard(TFunctionType& fn) : function_(fn) {}
  scope_guard(TFunctionType&& fn)
      : function_(std::forward<TFunctionType>(fn)) {}
  ~scope_guard() noexcept(false) {
    if (!dismissed_) {
      if (TNoexcept) {
        function_();
      } else {
        try {
          function_();
        } catch (...) {
          // todo: log
        }
      }
    }
  }
  void dismiss() { dismissed_ = true; }

 private:
  TFunctionType function_;
  bool dismissed_ = false;
};

template <typename TFunctionType>
struct is_noexcept_scope_guard {
  static constexpr bool value = noexcept(std::declval<TFunctionType>()());
};

template <typename TFunctionType>
auto make_scope_guard(TFunctionType&& fn) {
  return scope_guard<TFunctionType,
                     is_noexcept_scope_guard<TFunctionType>::value>(
      std::forward<TFunctionType>(fn));
}

}  // namespace wutils
