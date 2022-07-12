#ifndef TB_FUNCTION_REF_
#define TB_FUNCTION_REF_

#include <functional>
#include <type_traits>

#include "boost/call_traits.hpp"

namespace tb::detail {

template <bool IsConst, bool IsNoexcept, class R, class... Args>
class function_ref_impl {
 private:
  using fun_ptr_t = R (*)(Args...);

  template <class T>
  using propagate_const_t = std::conditional_t<IsConst, std::add_const_t<T>, T>;

  template <class T>
  static constexpr auto make_invoker() noexcept {
    return [](void* obj_ptr, Args... args) noexcept(IsNoexcept) -> R {
      std::invoke(
          *reinterpret_cast<std::add_pointer_t<propagate_const_t<T>>>(obj_ptr),
          args...);
    };
  }

 public:
 private:
  void* fun_ptr_;
  fun_ptr_t invoker_;
};

}  // namespace tb::detail

#endif /* TB_FUNCTION_REF_ */
