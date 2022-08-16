find_package(Catch2 3 REQUIRED)
if (NOT Catch2_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.1.0)

  FetchContent_MakeAvailable(Catch2)
endif ()

add_executable(tb_function_ref_test ${CMAKE_CURRENT_LIST_DIR}/main.cc)
target_link_libraries(tb_function_ref_test PRIVATE Catch2::Catch2WithMain tb_function_ref)

