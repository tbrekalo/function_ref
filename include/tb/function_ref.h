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
  static constexpr auto is_noexcept_compliant =
      std::conditional_t<IsNoexcept, std::is_nothrow_invocable_r<R, T, Args...>,
                         std::is_invocable_r<R, T, Args...>>::value;

  template <class T>
  using enable_if_compliant =
      std::enable_if_t<std::is_function_v<T> && is_noexcept_compliant<T>>;

 public:
  constexpr function_ref_impl() = delete;

  template <class F, class = enable_if_compliant<F>>
  explicit constexpr function_ref_impl(F&& fun) noexcept
      : fun_ptr_(std::addressof(fun)), invoker_(make_invoker<F>()) {}

  constexpr auto operator()(Args... args) noexcept(IsNoexcept) -> R {
    return invoker_(fun_ptr_, std::move(args)...);
  }

 protected:
  void* fun_ptr_;
  fun_ptr_t invoker_;
};

}  // namespace tb::detail

namespace tb {

template <class Sig>
class function_ref;

#define FN_REF_SPEC(is_noexcept, ...)                                         \
  template <class R, class... Args>                                           \
  class function_ref<R(Args...) __VA_ARGS__>                                  \
      : public detail::function_ref_impl<is_noexcept, R, Args...> {           \
    template <class F>                                                        \
    explicit constexpr function_ref(F&& fun)                                  \
        : detail::function_ref_impl<is_noexcept, R, Args...>(                 \
              std::forward<F>(fun)) {}                                        \
                                                                              \
    friend constexpr auto operator==(function_ref lhs,                        \
                                     function_ref rhs) noexcept -> bool {     \
      return lhs.fun_ptr_ == rhs.fun_ptr_;                                    \
    }                                                                         \
                                                                              \
    friend constexpr auto operator!=(function_ref lhs,                        \
                                     function_ref rhs) noexcept -> bool {     \
      return !(lhs == rhs);                                                   \
    }                                                                         \
                                                                              \
    friend constexpr auto swap(function_ref& lhs, function_ref& rhs) noexcept \
        -> void {                                                             \
      if (lhs != rhs) {                                                       \
        std::swap(lhs.fun_ptr_, rhs.fun_ptr_);                                \
      }                                                                       \
    }                                                                         \
  };

FN_REF_SPEC(false)
FN_REF_SPEC(true, noexcept)
#undef FN_REF_SPEC

}  // namespace tb

#endif /* TB_FUNCTION_REF_ */
