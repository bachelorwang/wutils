#pragma once
#include <iostream>

namespace wutils {

template <typename TFunctionType>
class scope_guard final {
 public:
  scope_guard() = delete;
  scope_guard(TFunctionType&& fn)
      : function_(std::forward<TFunctionType>(fn)) {}
  ~scope_guard() noexcept(true) {
    if (!dismissed_) {
      if (noexcept(std::declval<TFunctionType>()())) {
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
auto make_scope_guard(TFunctionType&& fn) {
  return scope_guard<TFunctionType>(std::forward<TFunctionType>(fn));
}

}  // namespace wutils
