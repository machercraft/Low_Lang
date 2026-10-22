cmake_minimum_required(VERSION 4.2.2)
project(low_lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        low_lang main.cpp low_lang.cpp low_lang_compiller.cpp
)
