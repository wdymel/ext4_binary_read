#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>

#include <errno.h>

#include "interfaces.h"
#include "flags.h"
#include <inttypes.h>
#include "structs/super_block.h"
#include "structs/group_descriptor.h"
#include "structs/inode_table.h"

static const char DRIVE_MOUNT[] = "binary.img"; // path on which the partition is mounted from

int load_first_super_block(FILE * file, struct SuperBlock * superBlock)
{
    static const uint sb_offset = 1024, sb_length = 1024;
    char buffer[sb_length];
    if (read_file_into_buffer(file, buffer, sb_offset, sb_length)) return 1;
    return SuperBblock_new(superBlock, buffer);
}

int load_group_descriptor(FILE * file, struct SuperBlock * superBlock, struct GroupDescriptor * groupDescriptor,
                          u_int64_t group_id)
// loads group descriptor for given block group
{
    if (superBlock->s_feature_incompat & INCOMPAT_META_BG) return 1;  // meta block groups are not supported
    // ALL group descriptor tables are located in first(0) block group
    // other block groups MAY store copies of them

    // locate block in which our group descriptor is in
    uint64_t block_offset = (group_id * superBlock->s_desc_size) / superBlock->s_block_size;
    // locate byte offset in block
    uint64_t byte_offset = (group_id * superBlock->s_desc_size) % superBlock->s_block_size;
    char * buffer = malloc(superBlock->s_block_size);
    if (read_block(file, superBlock->s_block_size, superBlock->s_first_group_desc_block + block_offset, buffer)) return 1;
    return GroupDescriptor_new(groupDescriptor, buffer + byte_offset, superBlock->s_desc_size);
}

int main() {
    FILE * file;
    errno = 0;
    file = fopen(DRIVE_MOUNT, "r");
    if (file == NULL)
    {
        fprintf(stderr, "error opening device_file errno=%d\n", errno);
        exit(1);
    }
    struct SuperBlock superBlock;
    load_first_super_block(file, &superBlock);

    if (superBlock.s_feature_incompat & REQUIRED_FEATURE_FLEX_BLOCK_GROUPS) printf("SYSTEM USES FLEX GROUPS\n");
    if (superBlock.s_feature_incompat & REQUIRED_FEATURE_64BIT) printf("SYSTEM USES 64BIT FEATURE\n");
    // reading inode no.2 containing root directory
    unsigned int inode_id = 2;
    unsigned int block_group = (inode_id - 1) / superBlock.s_inodes_per_group;
    unsigned int index = (inode_id - 1) % superBlock.s_inodes_per_group;
    unsigned int containing_block = (index * superBlock.s_inode_size) / superBlock.s_block_size;

    struct GroupDescriptor groupDescriptor;
    load_group_descriptor(file, &superBlock, &groupDescriptor, 0);
//
//    struct GroupDescriptor groupDescriptor2;
//    uint8_t i;
//    for (i = 0; i <= 6; ++i )
//    {
//        load_group_descriptor(file, &superBlock, &groupDescriptor2, i);
//        printf("%" PRIu8 ":\t" "%" PRIu32 "\n", i, groupDescriptor2.bg_free_blocks_count_u32);
//    }

    // To get the byte address within the inode table, use offset = index * sb->s_inode_size.
    u_int64_t offset = index * superBlock.s_inode_size;
    u_int64_t inode_table_block_address = groupDescriptor.bg_inode_table_u64 * superBlock.s_block_size;
    struct InodeTable inodeTable;
    {
        u_int64_t _buffer_len = superBlock.s_inode_size;
        char * buffer = malloc(_buffer_len);
//        read_block(file, superBlock.s_inode_size, inode_table_block_address + offset, buffer);
        read_file_into_buffer(file, buffer, inode_table_block_address + offset, _buffer_len);
        InodeTable_new(&inodeTable, buffer);
        free(buffer);
    }

    fclose(file);
    return 0;
}
