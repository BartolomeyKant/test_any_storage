cmake_minimum_required(VERSION 3.30)

project(test_any_storage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CACHE COUNT STRING "Integer: count of Foos" "")

if(NOT "${COUNT}" STREQUAL "")
  add_compile_definitions("-DCOUNT=${COUNT}")
endif()

add_library(manage_table INTERFACE)
target_include_directories(manage_table INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)

add_library(vtable INTERFACE)
target_include_directories(vtable INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)

add_library(inherit_table INTERFACE)
target_include_directories(inherit_table INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)


add_executable(test_manage_table src/test_manage_table.cpp)
target_link_libraries(test_manage_table PRIVATE manage_table)

add_executable(test_vtable src/test_vtable.cpp)
target_link_libraries(test_vtable PRIVATE vtable)

add_executable(test_inherit_table src/test_inheritance_table.cpp)
target_link_libraries(test_inherit_table PRIVATE inherit_table)
