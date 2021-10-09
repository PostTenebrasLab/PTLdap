cmake_minimum_required(VERSION 3.12)
project(ptldap/tests)

include_directories(toolbox)
include_directories(../include)

add_executable(tests 
    toolbox/test_main.cpp # Used to cache Catch2 build
    ber.cpp
    ldap.cpp
)
