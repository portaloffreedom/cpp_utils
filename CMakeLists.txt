cmake_minimum_required (VERSION 3.7.0)

project(cpp_utils)

set(CMAKE_CXX_STANDARD 17)

add_library(cpp_utils STATIC
        Address.cpp Address.h
)

set_property(TARGET cpp_utils
        PROPERTY POSITION_INDEPENDENT_CODE ON
)
