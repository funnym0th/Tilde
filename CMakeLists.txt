cmake_minimum_required(VERSION 3.10)

project(tilde)

add_executable(tilde main.cpp)

install(TARGETS tilde RUNTIME DESTINATION bin)
