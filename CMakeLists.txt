cmake_minimum_required(VERSION 3.18)
project(uloha6 C)

set(CMAKE_C_STANDARD 11)

add_executable(uloha6 main.c)
target_link_libraries(uloha6 pthread)
