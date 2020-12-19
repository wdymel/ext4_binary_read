//
// Created by wdymel on 2020-11-11.
//
#include <stdio.h>
#include <stdlib.h>
#ifndef PART_1_INTERFACES_H
#define PART_1_INTERFACES_H

#endif //PART_1_INTERFACES_H

int read_file_into_buffer(FILE * file, char * buffer, u_int64_t file_offset, u_int64_t read_length);
u_int64_t convert_le_byte_array_to_uint(const char * byte_array, int number_of_bytes);
int read_block(FILE * file, u_int64_t block_size, u_int64_t block_id, char * buffer);