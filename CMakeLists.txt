cmake_minimum_required(VERSION 3.16)
project(ext4_binary_read C)

set(CMAKE_C_STANDARD 99)

add_executable(ext4_binary_read main.c flags.h interfaces.h interfaces.c structs/super_block.c structs/super_block.h structs/group_descriptor.c structs/group_descriptor.h structs/inode_table.c structs/inode_table.h)

target_link_libraries(ext4_binary_read m)