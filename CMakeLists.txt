cmake_minimum_required(VERSION 2.6)
project(einkleinflug)

add_executable(einkleinflug main.cpp)

install(TARGETS einkleinflug RUNTIME DESTINATION bin)
