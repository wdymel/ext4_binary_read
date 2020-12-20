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

static const char DRIVE_MOUNT[] = "binary2.img"; // path on which the partition is mounted from

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

int load_inode_table(FILE * file, struct SuperBlock * superBlock, struct InodeTable * inodeTable, uint64_t inode_id)
{
    // locate block group this inode belongs to
    uint64_t block_group = (inode_id - 1) / superBlock->s_inodes_per_group;
    uint64_t index = (inode_id - 1) % superBlock->s_inodes_per_group;
    uint64_t containing_block = (index * superBlock->s_inode_size) / superBlock->s_block_size;
    uint64_t byte_offset = (index * superBlock->s_inode_size) % superBlock->s_block_size;
    struct GroupDescriptor groupDescriptor;
    if (load_group_descriptor(file, superBlock, &groupDescriptor, block_group)) return 1;
    char * buffer = malloc(superBlock->s_block_size);
    uint8_t err = read_block(file, superBlock->s_block_size, groupDescriptor.bg_inode_table_u64 + containing_block, buffer);
    if (!err)
        InodeTable_new(inodeTable, buffer + byte_offset);
    free(buffer);
    return err;
}

void print_block(char * buffer, struct SuperBlock * superBlock)
{
    for (uint i = 0; i < superBlock->s_block_size; ++i)
    {
        printf("%02x ", (u_char)buffer[i]);
        if (i % 16 == 15)
            printf("\n");
    }
}

int print_directory(FILE * file, struct SuperBlock * superBlock, struct InodeTable * inodeTable)
{
    if ((inodeTable->i_mode & S_IFDIR) == 0) return 1; // if given inode is not a directory
    if (inodeTable->i_flags & EXT4_INDEX_FL) printf("HASH TREE DIRECTORY");
    // load extent header
    struct ext4_extent_header ext4_extent_header;
    if (ext4_extent_header_new(&ext4_extent_header, inodeTable->i_block + 0x0)) return 1;
    // load 1st extent entry TODO add support for inodes spanning multiple extents
    struct ext4_extent ext4_extent;
    ext4_extent_new(&ext4_extent, inodeTable->i_block + 12 * 1);
    // print out directory
    char * buffer = malloc(superBlock->s_block_size);  // for storing blocks
    printf("file_type, name, inode\n");
    for (uint block = 0; block < ext4_extent.ee_len; ++block)  // iterate over blocks this inode occupies
    {
        read_block(file, superBlock->s_block_size, ext4_extent.ee_start_u64 + block, buffer);
        uint64_t pointer = 0;
        while(pointer < superBlock->s_block_size)
            // last 12 bytes is a phony ext4_dir_entry, which in fact is an ext4_dir_entry_tail which contains checksum
        {
            struct ext4_dir_entry_2 ext4_dir_entry_2;
            ext4_dir_entry_2_new(&ext4_dir_entry_2, buffer + pointer );
            if (ext4_dir_entry_2.inode)
                printf("%02x \"%.*s\" %d\n", ext4_dir_entry_2.file_type, ext4_dir_entry_2.name_len, ext4_dir_entry_2.name, ext4_dir_entry_2.inode);
            pointer += ext4_dir_entry_2.rec_len;
            free(ext4_dir_entry_2.name);
        }
    }
    free(buffer);
    return 0;
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
    if (superBlock.s_feature_incompat & INCOMPAT_FILETYPE) printf("INCOMPAT_FILETYPE filesystem uses ext4_dir_entry_2\n");
    if (superBlock.s_feature_incompat & INCOMPAT_DIRDATA) printf("INCOMPAT_DIRDATA\n");



    // load root directory (inode no. 2 is always root)
    struct InodeTable inodeTable;
    load_inode_table(file, &superBlock, &inodeTable, 260097);

    print_directory(file, &superBlock, &inodeTable);

//    if (inodeTable.i_flags & EXT4_INDEX_FL)
//        printf("Inode using hash trees");
//
//    // load extent header
//    struct ext4_extent_header ext4_extent_header;
//    ext4_extent_header_new(&ext4_extent_header, inodeTable.i_block + 0x0);
//
//    // load 1st extent entry
//    struct ext4_extent ext4_extent;
//    ext4_extent_new(&ext4_extent, inodeTable.i_block + 12 * 1);
//
//    // load data block that extent is pointing at
//    char * buffer = malloc(superBlock.s_block_size);
//    read_block(file, superBlock.s_block_size, ext4_extent.ee_start_u64, buffer);
//    print_block(buffer, &superBlock);
//
//    // load linear directory header
//    uint64_t pointer = 0;
//    while(pointer < superBlock.s_block_size - 12)
//    {
//        struct ext4_dir_entry_2 ext4_dir_entry_2;
//        ext4_dir_entry_2_new(&ext4_dir_entry_2, buffer + pointer );
//        printf("%02x \"%.*s\" %d\n", ext4_dir_entry_2.file_type, ext4_dir_entry_2.name_len, ext4_dir_entry_2.name, ext4_dir_entry_2.inode);
//        pointer += ext4_dir_entry_2.rec_len;
//        free(ext4_dir_entry_2.name);
//    }  // last 12 bytes is a phony ext4_dir_entry, which in fact is ext4_dir_entry_tail
//
//
//    struct InodeTable test_file_inode_tables;
//    load_inode_table(file, &superBlock, &test_file_inode_tables, 13);
//
//    // load extent header
//    struct ext4_extent_header test_file_extent_header;
//    ext4_extent_header_new(&test_file_extent_header, test_file_inode_tables.i_block + 0x0);
//
//    // load 1st extent entry
//    struct ext4_extent test_file_extent;
//    ext4_extent_new(&test_file_extent, test_file_inode_tables.i_block + 12 * 1);
//    read_block(file, superBlock.s_block_size, test_file_extent.ee_start_u64, buffer);
//    print_block(buffer, &superBlock);
//
//    free(buffer);

    fclose(file);
    return 0;
}
