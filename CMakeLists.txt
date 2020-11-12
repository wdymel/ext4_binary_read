cmake_minimum_required(VERSION 3.16)
project(ext4_binary_read C)

set(CMAKE_C_STANDARD 99)

add_executable(ext4_binary_read main.c flags.h structs.h structs.c interfaces.h interfaces.c)

target_link_libraries(ext4_binary_read m)