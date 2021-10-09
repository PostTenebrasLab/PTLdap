cmake_minimum_required(VERSION 3.12)
project(ptldap/examples)

include_directories(libs/brynet/include)
include_directories(../include)

add_executable(example_ldap_search ldap_search.cpp)
target_link_libraries(example_ldap_search pthread)
