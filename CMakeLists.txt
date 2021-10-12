cmake_minimum_required(VERSION 3.12)
project(ptldap)

add_library(ptldap INTERFACE)
target_include_directories(ptldap INTERFACE include)
target_compile_features(ptldap INTERFACE cxx_std_20)

add_executable(tests
    test/main.cpp
    test/ber.cpp
    test/ldap.cpp
)
target_include_directories(tests PRIVATE test)
target_link_libraries(tests PRIVATE ptldap)

add_executable(example_ldap_search
    examples/ldap_search.cpp
)
target_include_directories(example_ldap_search PRIVATE examples/libs/brynet/include)
target_link_libraries(example_ldap_search PRIVATE ptldap pthread)
