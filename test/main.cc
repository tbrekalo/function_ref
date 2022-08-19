#include <stdexcept>

#include "catch2/catch_test_macros.hpp"
#include "tb/function_ref.h"

static constexpr auto kIntConstant = 42;

static auto foo() -> int { return kIntConstant; }
static auto bar(int& x) -> void { x += kIntConstant; }

TEST_CASE("int()_from_ptr") {
  auto foo_ref = tb::function_ref(&foo);
  CHECK(foo_ref() == foo());
}

TEST_CASE("int()_from_ref") {
  auto foo_ref = tb::function_ref(foo);
  CHECK(foo_ref() == foo());
}

TEST_CASE("void(int&)_from_ptr") {
  auto bar_ref = tb::function_ref(&bar);
  auto x = 0;

  bar_ref(x);
  CHECK(x == kIntConstant);
}

TEST_CASE("void(int&)_from_ref") {
  auto bar_ref = tb::function_ref(bar);
  auto x = 0;

  bar_ref(x);
  CHECK(x == kIntConstant);
}

TEST_CASE("int()_from_lambda") {
  auto lambda = [k = kIntConstant]() -> int { return k; };
  auto lambda_ref = tb::function_ref<int()>(lambda);
  auto lambda_const_ref = tb::function_ref<int() const>(lambda);

  CHECK(lambda_ref() == kIntConstant);
  CHECK(lambda_const_ref() == kIntConstant);
}

TEST_CASE("int()_noexcept_from_lambda") {
  auto lambda = [k = kIntConstant]() noexcept -> int { return k; };
  auto lambda_ref = tb::function_ref<int() noexcept>(lambda);
  auto lambda_const_ref = tb::function_ref<int() const noexcept>(lambda);

  CHECK(lambda_ref() == kIntConstant);
  CHECK(lambda_const_ref() == kIntConstant);
}

TEST_CASE("void()_throws") {
  auto lambda = []() -> void { throw std::runtime_error("threw up"); };

  auto lambda_ref = tb::function_ref<void()>(lambda);
  CHECK_THROWS_AS(lambda_ref(), std::runtime_error);
}

TEST_CASE("int()_stateful_lambda") {
  auto lambda = [i = 0]() mutable -> int {
    if (i < 10) {
      return i++;
    } else {
      throw std::runtime_error("threw up");
    }
  };

  auto lambda_ref = tb::function_ref<int()>(lambda);

  for (auto i = 0; i < 10; ++i) {
    CHECK(lambda() == i);
  }

  CHECK_THROWS_AS(lambda(), std::runtime_error);
}
