#ifndef TB_FUNCTION_REF_
#define TB_FUNCTION_REF_

#include <functional>
#include <memory>
#include <type_traits>

namespace tb::detail {

template <class Signature>
struct function_ref_traits;

template <class R, class... Args>
struct function_ref_traits<R(Args...)> {
  static auto constexpr is_noexcept = false;

  using signature = R(Args...);

  template <class Fn>
  static constexpr auto is_compatible_v = std::is_invocable_r_v<R, Fn, Args...>;
};

struct function_ref_base {
  union storage_t {
    constexpr storage_t() noexcept = default;

    template <class T, class = std::enable_if_t<std::is_object_v<T>>>
    constexpr storage_t(T* t) noexcept : obj_ptr_(t) {}

    template <class T, class = std::enable_if_t<std::is_object_v<T>>>
    constexpr storage_t(T const* t) noexcept : obj_ptr_const_(t) {}

    template <class T,
              std::enable_if_t<std::is_function_v<T>, std::nullptr_t> = nullptr>
    constexpr storage_t(T* t) noexcept
        : fun_ptr_(reinterpret_cast<void (*)()>(t)) {}

    void* obj_ptr_ = nullptr;
    void const* obj_ptr_const_;
    void (*fun_ptr_)();
  };

  template <class T>
  static constexpr auto get(storage_t storage) noexcept {
    if constexpr (std::is_function_v<T>) {
      return storage.fun_ptr_;
    } else if (std::is_const_v<T>) {
      return storage.obj_ptr_const_;
    } else if (std::is_object_v<T>) {
      return storage.obj_ptr_;
    }
  }
};

}  // namespace tb::detail

namespace tb {

template <class Signature>
class function_ref;

template <class R, class... Args>
class function_ref<R(Args...)> : detail::function_ref_base {
  using traits = detail::function_ref_traits<R(Args...)>;

  using invoker_t = R(storage_t, Args...);
  using invoker_ptr_t = std::add_pointer_t<invoker_t>;

 public:
  template <class F,
            class = std::enable_if_t<traits::template is_compatible_v<F> and
                                     std::is_function_v<F>>>
  explicit constexpr function_ref(F* fun_ptr)
      : storage_(fun_ptr),
        invoker_ptr_([](storage_t storage,
                        Args... args) noexcept(traits::is_noexcept) -> R {
          return std::invoke(reinterpret_cast<F*>(get<F>(storage)),
                             std::forward<Args>(args)...);
        }) {}

  constexpr auto operator()(Args... args) const noexcept(traits::is_noexcept)
      -> R {
    return invoker_ptr_(storage_, std::forward<Args>(args)...);
  }

 private:
  storage_t storage_;
  invoker_ptr_t invoker_ptr_;
};

}  // namespace tb

#endif /* TB_FUNCTION_REF_ */
