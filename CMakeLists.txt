cmake_minimum_required(VERSION 3.13)
project(function_ref VERSION 0.1.0)


add_library(tb_function_ref INTERFACE)
add_library(tb::function_ref ALIAS tb_function_ref)
target_include_directories(tb_function_ref INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(tb_function_ref INTERFACE cxx_std_20)

find_package(Boost 1.66 REQUIRED)
target_include_directories(tb_function_ref INTERFACE ${Boost_INCLUDE_DIRS})

add_executable(test test/main.cc)
target_link_libraries(test tb_function_ref)
