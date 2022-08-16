#ifndef TB_FUNCTION_REF_
#define TB_FUNCTION_REF_

#include <functional>
#include <memory>
#include <type_traits>

namespace tb::detail {

template <bool IsNoexcept, class R, class... Args>
class function_ref_impl {
 private:
  using fun_ptr_t = R (*)(void*, Args...) noexcept(IsNoexcept);

  template <class T>
  static constexpr auto make_invoker() noexcept -> fun_ptr_t {
    return [](void* fun_ptr, Args... args) noexcept(IsNoexcept) -> R {
      std::invoke(*reinterpret_cast<std::add_pointer_t<T>>(fun_ptr), args...);
    };
  }

  template <class T>
  static constexpr bool is_noexcept_compliant =
      std::conditional_t<IsNoexcept, std::is_nothrow_invocable_r<R, T, Args...>,
                         std::is_invocable_r<R, T, Args...>>::value;

  template <class T>
  using enable_if_compliant =
      std::enable_if_t<std::is_function_v<T> && is_noexcept_compliant<T>>;

 public:
  constexpr function_ref_impl() = delete;

  template <class F, class = enable_if_compliant<F>>
  constexpr function_ref_impl(F&& fun) noexcept
      : fun_ptr_(std::addressof(fun)), invoker_(make_invoker<F>()) {}

  constexpr auto operator()(Args... args) noexcept(IsNoexcept) -> R {
    return invoker_(fun_ptr_, std::move(args)...);
  }

 private:
  void* fun_ptr_;
  fun_ptr_t invoker_;
};

}  // namespace tb::detail

namespace tb {

template <class Sig>
class function_ref;

template <class R, class... Args>
class function_ref<R(Args...)>
    : public detail::function_ref_impl<false, R, Args...> {};

template <class R, class... Args>
class function_ref<R(Args...) noexcept>
    : public detail::function_ref_impl<true, R, Args...> {};

}  // namespace tb

#endif /* TB_FUNCTION_REF_ */
