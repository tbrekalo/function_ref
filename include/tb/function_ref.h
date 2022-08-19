#ifndef TB_FUNCTION_REF_
#define TB_FUNCTION_REF_

/*
  "Bad artists copy. Good artists steal."
    - Pablo Picasso
*/

#include <memory>
#include <type_traits>

namespace tb::detail {

template <class Signature>
struct function_ref_traits;

template <class R, class... Args>
struct function_ref_traits<R(Args...)> {
  static constexpr auto is_noexcept = false;

  template <class T>
  using cv = T;

  template <class Fn>
  static constexpr auto is_compatible_v = std::is_invocable_r_v<R, Fn, Args...>;
};

template <class R, class... Args>
struct function_ref_traits<R(Args...) noexcept> {
  static constexpr auto is_noexcept = true;

  template <class T>
  using cv = T;

  template <class Fn>
  static constexpr auto is_compatible_v =
      std::is_nothrow_invocable_r_v<R, Fn, Args...>;
};

template <class R, class... Args>
struct function_ref_traits<R(Args...) const> : function_ref_traits<R(Args...)> {
  template <class T>
  using cv = T const;
};

template <class R, class... Args>
struct function_ref_traits<R(Args...) const noexcept>
    : function_ref_traits<R(Args...) noexcept> {
  template <class T>
  using cv = T const;
};

struct function_ref_base {
  union storage_t {
    constexpr storage_t() noexcept = default;

    template <class T, std::enable_if_t<std::is_object_v<T>, int> = 0>
    constexpr storage_t(T* t) noexcept : obj_ptr_(t) {}

    template <class T, std::enable_if_t<std::is_const_v<T>, int> = 1>
    constexpr storage_t(T* t) noexcept : obj_ptr_const_(t) {}

    template <class T, std::enable_if_t<std::is_function_v<T>, int> = 2>
    constexpr storage_t(T* t) noexcept
        : fun_ptr_(reinterpret_cast<void (*)()>(t)) {}

    void* obj_ptr_ = nullptr;
    void const* obj_ptr_const_;
    void (*fun_ptr_)();
  };

  template <class T>
  static constexpr auto get(storage_t storage) noexcept {
    if constexpr (std::is_function_v<T>) {
      return reinterpret_cast<T*>(storage.fun_ptr_);
    } else if constexpr (std::is_const_v<T>) {
      return static_cast<T*>(storage.obj_ptr_const_);
    } else if constexpr (std::is_object_v<T>) {
      return static_cast<T*>(storage.obj_ptr_);
    }
  }
};

}  // namespace tb::detail

namespace tb {

template <class Signature>
class function_ref;

#define FUN_REF(...)                                                           \
  template <class R, class... Args>                                            \
  class function_ref<R(Args...) __VA_ARGS__> : detail::function_ref_base {     \
    using traits = detail::function_ref_traits<R(Args...) __VA_ARGS__>;        \
                                                                               \
    template <class T>                                                         \
    using ref_t = typename traits::template cv<T>&;                            \
                                                                               \
    using invoker_t = R(storage_t, Args...);                                   \
    using invoker_ptr_t = std::add_pointer_t<invoker_t>;                       \
                                                                               \
   public:                                                                     \
    template <class F, class = std::enable_if_t<                               \
                           traits::template is_compatible_v<F> and             \
                           not std::is_member_function_pointer_v<F> and        \
                           std::is_function_v<F>>>                             \
    explicit constexpr function_ref(F* fun_ptr) noexcept                       \
        : storage_(fun_ptr),                                                   \
          invoker_ptr_([](storage_t storage,                                   \
                          Args... args) noexcept(traits::is_noexcept) -> R {   \
            if constexpr (std::is_same_v<R, void>) {                           \
              get<F>(storage)(std::forward<Args>(args)...);                    \
            } else {                                                           \
              return get<F>(storage)(std::forward<Args>(args)...);             \
            }                                                                  \
          }) {}                                                                \
                                                                               \
    template <class F,                                                         \
              class = std::enable_if_t<traits::template is_compatible_v<F> and \
                                       std::is_object_v<F> and                 \
                                       not std::is_same_v<function_ref, F>>>   \
    explicit constexpr function_ref(F& fun_obj) noexcept                       \
        : storage_(std::addressof(fun_obj)),                                   \
          invoker_ptr_([](storage_t storage,                                   \
                          Args... args) noexcept(traits::is_noexcept) -> R {   \
            ref_t<F> ref = *get<F>(storage);                                   \
            if constexpr (std::is_same_v<R, void>) {                           \
              ref(std::forward<Args>(args)...);                                \
            } else {                                                           \
              return ref(std::forward<Args>(args)...);                         \
            }                                                                  \
          }) {}                                                                \
                                                                               \
    constexpr auto operator()(Args... args) const                              \
        noexcept(traits::is_noexcept) -> R {                                   \
      return invoker_ptr_(storage_, std::forward<Args>(args)...);              \
    }                                                                          \
                                                                               \
   private:                                                                    \
    storage_t storage_;                                                        \
    invoker_ptr_t invoker_ptr_;                                                \
  };

FUN_REF()
FUN_REF(noexcept)

FUN_REF(const)
FUN_REF(const noexcept)

#undef FUN_REF

template <class F>
function_ref(F*) -> function_ref<std::enable_if_t<std::is_function_v<F>, F>>;

}  // namespace tb

#endif /* TB_FUNCTION_REF_ */
