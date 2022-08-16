cmake_minimum_required(VERSION 3.21)
project(function_ref VERSION 0.1.0)

option(TB_FUNCTION_REF_BUILD_TEST "Build catch2 tests" OFF)
if (PROJECT_IS_TOP_LEVEL)
  set(TB_FUNCTION_REF_BUILD_TEST ON)
endif ()
 
add_library(tb_function_ref INTERFACE)
add_library(tb::function_ref ALIAS tb_function_ref)
target_include_directories(tb_function_ref INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(tb_function_ref INTERFACE cxx_std_17)

if (TB_FUNCTION_REF_BUILD_TEST)
  include(${CMAKE_CURRENT_LIST_DIR}/test/FunctionRefTest.cmake) 
endif ()
