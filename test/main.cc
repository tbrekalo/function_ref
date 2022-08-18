#include "tb/function_ref.h"
#include "catch2/catch_test_macros.hpp"

static auto foo() -> int {
  return 42;
}

TEST_CASE("play") {
  auto foo_ref = tb::function_ref<int()>(&foo);
  CHECK(foo_ref() == foo());
}
