#pragma once

namespace wutils {

class non_copy_assign {
 public:
  non_copy_assign() = default;
  virtual ~non_copy_assign() = default;

  non_copy_assign(const non_copy_assign&) = delete;
  non_copy_assign& operator=(const non_copy_assign&) = delete;

  non_copy_assign(const non_copy_assign&&) = delete;
  non_copy_assign& operator=(non_copy_assign&&) = delete;
};

}  // namespace wutils
