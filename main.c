#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <errno.h>

#include "structs.h"
#include "interfaces.h"
#include "flags.h"

static const char DRIVE_MOUNT[] = "home.img"; // path on which the partition is mounted from


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
    {
        static const uint sb_offset = 1024, sb_length = 1024;
        char buffer[sb_length];
        if (read_file_into_buffer(file, buffer, sb_offset, sb_length)) return 1;
        SuperBblock_new(&superBlock, buffer);
    }

    if (superBlock.required_features & REQUIRED_FEATURE_FLEX_BLOCK_GROUPS) printf("SYSTEM USES FLEX GROUPS\n");
    if (superBlock.required_features & REQUIRED_FEATURE_64BIT) printf("SYSTEM USES 64BIT FEATURE\n");
    // reading inode no.2 containing root directory
    unsigned int inode_id = 2;
    unsigned int block_group = (inode_id - 1) / superBlock.inodes_per_group;
    unsigned int index = (inode_id - 1) % superBlock.inodes_per_group;
    unsigned int containing_block = (index * superBlock.inode_size) / superBlock._log_block_size;

    struct GroupDescriptor groupDescriptor;
    {
        char * buffer = malloc(superBlock.block_size);
        read_block(file, superBlock.block_size, superBlock.group_desc_tab_block_addr, buffer);
        GroupDescriptor_new(&groupDescriptor, buffer, superBlock.group_desc_size);
        free(buffer);
    }

    // To get the byte address within the inode table, use offset = index * sb->s_inode_size.
    u_int64_t offset = index * superBlock.inode_size;
    u_int64_t inode_table_block_address = groupDescriptor.bg_inode_table_u64;
    struct InodeTable inodeTable;
    {
        char * buffer = malloc(superBlock.inode_size);
//        read_block(file, superBlock.inode_size, )
        free(buffer);
    }

    fclose(file);
    return 0;
}
