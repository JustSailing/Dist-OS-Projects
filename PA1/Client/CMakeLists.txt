cmake_minimum_required(VERSION 3.20)
project(Client)

set(CMAKE_CXX_STANDARD 20)

add_executable(Client main.cpp message.h client.cpp client.h logger.cpp logger.h)

target_link_libraries(Client -lpthread -lcrypto)