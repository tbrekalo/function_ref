#include "catch2/catch_test_macros.hpp"
#include "tb/function_ref.h"

static constexpr auto kIntConstant = 42;

static auto foo() -> int { return kIntConstant; }
static auto bar(int& x) -> void { x += kIntConstant; }

TEST_CASE("int()_from_ptr") {
  auto foo_ref = tb::function_ref<int()>(&foo);
  CHECK(foo_ref() == foo());
}

TEST_CASE("int()_from_ref") {
  auto foo_ref = tb::function_ref<int()>(foo);
  CHECK(foo_ref() == foo());
}

TEST_CASE("void(int&)_from_ptr") {
  auto bar_ref = tb::function_ref<void(int&)>(&bar);
  auto x = 0;

  bar_ref(x);
  CHECK(x == kIntConstant);
}

TEST_CASE("void(int&)_from_ref") {
  auto bar_ref = tb::function_ref<void(int&)>(bar);
  auto x = 0;

  bar_ref(x);
  CHECK(x == kIntConstant);
}

TEST_CASE("int()_from_lambda") {
  // auto lambda = [k = kIntConstant]() -> int { return k; };
  // tb::function_ref<int() const noexcept> lambda_ref(lambda);
}
