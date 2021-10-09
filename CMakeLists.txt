cmake_minimum_required(VERSION 3.12)
project(ptldap)

add_library(ptldap INTERFACE)
target_include_directories(ptldap INTERFACE include/)
target_compile_features(ptldap INTERFACE cxx_std_17)

add_subdirectory(examples)
add_subdirectory(tests)