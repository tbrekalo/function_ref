# function_ref
Yet another c++ `function_ref` implementation (honestly, reimplementation). 

`tb::function_ref<Signature>` aims to follow 
[P0792R8](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0792r8.html)
and is heavily inspired by it's implementation. The
[P0792R8](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0792r8.html)'s 
attached implementation relies on c++20 features and extra template parameters for black magic.
`tb::function_ref<Signature>` is bound to c++17 and uses the frowned upon
art of preprocessor macros.

## Usage
Integrate the library in your project using [CMake's FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) or just copy and paste the header file from `include/tb/function_ref.h`

### Example code 
```c++
auto lambda = [i = 0]() mutable -> int {
  return i++;
};

auto lambda_ref = tb::function_ref<int()>(lambda);

for (auto i = 0; i < 10; ++i) {
  do_smart_stuff(i);
}
```

### references
[P0792R8](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0792r8.html)
[Vittorio Romeo's implementation](https://github.com/vittorioromeo/Experiments/blob/master/function_ref.cpp)
[zhihaoy's implementation](https://github.com/zhihaoy/nontype_functional)
