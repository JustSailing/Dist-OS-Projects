cmake_minimum_required(VERSION 3.22)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server main.cpp server.cpp logger.cpp)
target_link_libraries(server -lcrypto -lpthread)
