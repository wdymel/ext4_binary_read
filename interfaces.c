//
// Created by wdymel on 2020-11-11.
//
#include "interfaces.h"

int read_file_into_buffer(FILE * file, char * buffer, long file_offset, unsigned long read_length)
// read <read_length> of bytes from stream into <buffer> starting from <file_offset> position
{
    unsigned long int read_size;
    // just some sanity checks
    if (file == NULL || buffer == NULL) return 1;

    if (fseek(file, file_offset, SEEK_SET) != 0) return 3; // move file cursor to specified position, relative to file's beginning
    read_size = fread(buffer, 1, read_length, file); // read file into buffer
    if (read_size != read_length) return 4;

    return 0;
}

u_int64_t convert_le_byte_array_to_uint(const char * byte_array, int number_of_bytes)
// convert little endian byte array to unsigned int
{
    u_int64_t result = 0;
    int i;
    if (number_of_bytes > sizeof(unsigned int)) fprintf(stderr, "Error, int size too small in convert_le_byte_array_to_uint"); //
    for (i=number_of_bytes -1; i >= 0; --i)
    {
        result = result << 8u;
        result += (unsigned char)byte_array[i];
    }
    return result;
}

int read_block(FILE * file, u_int64_t block_size, u_int64_t block_id, char * buffer)
{
    u_int64_t starting_pos = block_size * block_id;
    if (read_file_into_buffer(file, buffer, starting_pos, block_size))
        return 1;
    else return 0;
}