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
#include <string.h>

static const char DRIVE_MOUNT[] = "binary.img"; // path on which the partition is mounted from

int load_super_block(FILE * file, struct SuperBlock * superBlock)
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
    static uint ROW_LEN = 16;
    for (uint i = 0; i < superBlock->s_block_size; ++i)
    {
        printf("%02x ", (u_char)buffer[i]);
        if (i % ROW_LEN == ROW_LEN - 1)
            printf("\n");
    }
}

int get_directory(FILE * file, struct SuperBlock * superBlock, struct InodeTable * inodeTable, struct ext4_dir_entry_2 ** dir_entries)
{
    if ((inodeTable->i_mode & S_IFDIR) == 0) return 1; // if given inode is not a directory
    if (inodeTable->i_flags & EXT4_INDEX_FL) printf("HASH TREE DIRECTORY\n");
    // load extent header
    struct ext4_extent_header ext4_extent_header;
    if (ext4_extent_header_new(&ext4_extent_header, inodeTable->i_block + 0x0)) return 1;
    // load 1st extent entry TODO add support for inodes spanning multiple extents
    struct ext4_extent ext4_extent;
    ext4_extent_new(&ext4_extent, inodeTable->i_block + 12 * 1);
    char * buffer = malloc(superBlock->s_block_size);  // for storing blocks
    uint64_t entries_count = 0;
    for (uint block = 0; block < ext4_extent.ee_len; ++block)  // iterate over blocks this inode occupies
    // iterate over whole extent list to count number of directories
    {
        read_block(file, superBlock->s_block_size, ext4_extent.ee_start_u64 + block, buffer);
        uint64_t pointer = 0;
        while(pointer < superBlock->s_block_size)
            // last 12 bytes is a phony ext4_dir_entry, which in fact is an ext4_dir_entry_tail which contains checksum
        {
            struct ext4_dir_entry_2 ext4_dir_entry_2;
            ext4_dir_entry_2_new(&ext4_dir_entry_2, buffer + pointer, 0);
            if (ext4_dir_entry_2.inode)  // if entry is used it has non zero value
                entries_count += 1;
            pointer += ext4_dir_entry_2.rec_len;
        }
    }
    // alloc memory for all directory entries
    *dir_entries = malloc(sizeof(struct ext4_dir_entry_2) * entries_count);
//    struct dir_element * elements = malloc(sizeof(struct dir_element) * entries_count);
    uint64_t current_entry = 0;
    for (uint block = 0; block < ext4_extent.ee_len; ++block)  // iterate over blocks this inode occupies
    // iterate over directory again, this time coping names and inode numbers
    {
        read_block(file, superBlock->s_block_size, ext4_extent.ee_start_u64 + block, buffer);
        uint64_t pointer = 0;
        while(pointer < superBlock->s_block_size && current_entry < entries_count)
            // last 12 bytes is a phony ext4_dir_entry, which in fact is an ext4_dir_entry_tail which contains checksum
        {
            struct ext4_dir_entry_2 * ext4_dir_entry_2 = (*dir_entries) + current_entry;
            ext4_dir_entry_2_new(ext4_dir_entry_2, buffer + pointer, 1);
            if (ext4_dir_entry_2->inode)  // if entry is used it has non zero value
                current_entry += 1;
            else
                free(ext4_dir_entry_2->name);
            pointer += ext4_dir_entry_2->rec_len;
        }
    }
    free(buffer);
    return entries_count;
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
            ext4_dir_entry_2_new(&ext4_dir_entry_2, buffer + pointer, 1);
            if (ext4_dir_entry_2.inode)
                printf("%02x \"%.*s\" %d\n", ext4_dir_entry_2.file_type, ext4_dir_entry_2.name_len, ext4_dir_entry_2.name, ext4_dir_entry_2.inode);
            pointer += ext4_dir_entry_2.rec_len;
            free(ext4_dir_entry_2.name);
        }
    }
    free(buffer);
    return 0;
}

int cat_file(FILE * file, struct SuperBlock * superBlock, struct InodeTable * inodeTable)
{
    if ((inodeTable->i_mode & S_IFREG) == 0)
    {
        printf("Not a file!\n");
        return 1;
    }
    // load extent header
    struct ext4_extent_header ext4_extent_header;
    if (ext4_extent_header_new(&ext4_extent_header, inodeTable->i_block + 0x0)) return 1;
    if (ext4_extent_header.eh_depth == 0)
    {
        struct ext4_extent ext4_extent;
        ext4_extent_new(&ext4_extent, inodeTable->i_block + 12 * 1);
        uint64_t bytes_to_read = inodeTable->i_size_u64;
        char * buffer = malloc(superBlock->s_block_size);
        for (uint64_t block = 0; block < ext4_extent.ee_len; ++block)
        {
            read_block(file, superBlock->s_block_size, ext4_extent.ee_start_u64 + block, buffer);
            print_block(buffer, superBlock);
        }
    }
    else if (ext4_extent_header.eh_depth == 1)
    {
        struct ext4_extent_idx ext4_extent_idx;
        ext4_extent_idx_new(&ext4_extent_idx, inodeTable->i_block + 12 * 1);
        char * buffer = malloc(superBlock->s_block_size);
        read_block(file, superBlock->s_block_size, ext4_extent_idx.ei_leaf_u64, buffer);
        struct ext4_extent_header header2;
        if (ext4_extent_header_new(&header2, buffer + 0x0)) return 1;
        for (uint64_t block = 0; block < header2.eh_entries; ++block)
        {

        }
        uint8_t x = 1;
    }
    return 0;
}

int shell_load_directory(FILE * file, struct SuperBlock * superBlock, struct InodeTable * inodeTable,
        uint64_t * current_inode_id, uint64_t next_inode_id)
{
    if (load_inode_table(file, superBlock, inodeTable, next_inode_id))
    {
        printf("Error loading directory\n");
        return load_inode_table(file, superBlock, inodeTable, *current_inode_id);
    }
    else
        *current_inode_id = next_inode_id;
    return 0;
}

int inode_block_recursive(FILE * file, struct SuperBlock * superBlock, struct ext4_extent_header * extent_header,
        u_char * extent_data, uint64_t * blocks, uint64_t * blocks_read)
{
    if (extent_header->eh_depth == 0)
    {
        for(uint64_t leaf_num = 0; leaf_num < extent_header->eh_entries; ++leaf_num)
        {
            struct ext4_extent leaf;
            ext4_extent_new(&leaf, (char*)extent_data + 12 + 12 * leaf_num);
            for (uint64_t block_num = 0; block_num < leaf.ee_len; ++block_num)
            {
                blocks[*blocks_read] = leaf.ee_start_u64 + block_num;
//                printf("%" PRIu64 "\n", leaf.ee_start_u64 + block_num);
                *blocks_read += 1;
            }
        }
    }
    else
    {
        u_char * leaf_data = malloc(superBlock->s_block_size * sizeof(u_char));
        for(uint64_t index_num = 0; index_num < extent_header->eh_entries; ++index_num)
        {
            struct ext4_extent_idx index;
            ext4_extent_idx_new(&index, (char*)extent_data + 12 + 12 * index_num);
            read_block(file, superBlock->s_block_size, index.ei_leaf_u64, leaf_data);
            struct ext4_extent_header header;
            if (ext4_extent_header_new(&header, leaf_data))
            {
                printf("Error reading extent header from block num %" PRIu64 "\n", index.ei_leaf_u64);
                return 1;
            }
            if (inode_block_recursive(file, superBlock, &header, leaf_data, blocks, blocks_read))
                return 1;
        }
    }
    return 0;
}

int get_inode_block_list(FILE * file, struct SuperBlock * superBlock, struct InodeTable * inodeTable,
        uint64_t ** blocks, uint64_t * blocks_count)
{
    uint64_t sector_size_in_block_count;
    uint64_t blocks_in_inode;
    if (!(superBlock->s_feature_ro_compat & 0x8u))
    {
        sector_size_in_block_count = 512;
        blocks_in_inode = inodeTable->i_blocks_lo;
    }
    else if (!(inodeTable->i_flags & EXT4_HUGE_FILE_FL))
    {
        sector_size_in_block_count = 512;
        blocks_in_inode = inodeTable->i_blocks_lo + ((uint64_t)inodeTable->l_i_blocks_high << 32u);
    }
    else
    {
        sector_size_in_block_count = superBlock->s_block_size;
        blocks_in_inode = ((uint64_t)inodeTable->i_blocks_lo + (uint64_t)inodeTable->l_i_blocks_high) << 32u;
    }
    blocks_in_inode /= superBlock->s_block_size / sector_size_in_block_count;

    *blocks = malloc(sizeof(uint64_t) * blocks_in_inode);
    struct ext4_extent_header header;
    if (ext4_extent_header_new(&header, (char*)inodeTable->i_block + 0x0)) return 1;
    uint64_t blocks_read = 0;
    inode_block_recursive(file, superBlock, &header, inodeTable->i_block, *blocks, &blocks_read);
    if (blocks_in_inode != blocks_read)
        printf("Number of read blocks in inode " "is not equal to number of blocks declared in inode table\n");
    *blocks_count = blocks_read;
    return 0;
}

int get_directory_list(FILE * file, struct SuperBlock * superBlock, struct InodeTable * inodeTable,
        struct ext4_dir_entry_2 ** dir_entries, uint64_t * dir_entries_count)
{
    if ((inodeTable->i_mode & S_IFDIR) == 0) return 1; // if given inode is not a directory
    if (inodeTable->i_flags & EXT4_INDEX_FL) printf("HASH TREE DIRECTORY\n");
    uint64_t * blocks;
    uint64_t block_count;
    if (get_inode_block_list(file, superBlock, inodeTable, &blocks, &block_count))
    {
        printf("Error getting inode block list\n");
        return 1;
    }
    char * block_data = malloc(superBlock->s_block_size * sizeof(u_char));
    *dir_entries_count = 0;
    for (uint64_t i = 0; i < block_count; ++i)
    {
        read_block(file, superBlock->s_block_size, blocks[i], block_data);
        uint64_t pointer = 0;
        while(pointer < superBlock->s_block_size)
        {
            struct ext4_dir_entry_2 dir_entry;
            ext4_dir_entry_2_new(&dir_entry, block_data + pointer, 0);
            if (dir_entry.inode)
                *dir_entries_count += 1;
            pointer += dir_entry.rec_len;
        }
    }
    *dir_entries = malloc(sizeof(struct ext4_dir_entry_2) * *dir_entries_count);
    uint64_t current_entry = 0;
    for (uint64_t i = 0; i < block_count && current_entry < *dir_entries_count; ++i)
    {
        read_block(file, superBlock->s_block_size, blocks[i], block_data);
        uint64_t pointer = 0;
        while(pointer < superBlock->s_block_size && current_entry < *dir_entries_count)
        {
            struct ext4_dir_entry_2 * dir_entry = (*dir_entries) + current_entry;
            ext4_dir_entry_2_new(dir_entry, block_data + pointer, 1);
            if (dir_entry->inode)
                current_entry += 1;
            else
                free(dir_entry->name);
            pointer += dir_entry->rec_len;
        }
    }
    return 0;
}

int find_path_in_directory(FILE * file, struct SuperBlock * superBlock, struct InodeTable * current_directory,
        char * file_name, struct ext4_dir_entry_2 * found_dir_entry)
{
    struct ext4_dir_entry_2 * dir_entries;
    uint64_t file_name_len = strlen(file_name);
    uint64_t elements_count;
    get_directory_list(file, superBlock, current_directory, &dir_entries, &elements_count);
    uint8_t found_dir = 0;
    for (uint64_t i = 0; i < elements_count; ++i)
    {
        if (dir_entries[i].name_len == file_name_len && strncmp(file_name, dir_entries[i].name, dir_entries[i].name_len) == 0)
        {
            memcpy(found_dir_entry, dir_entries + i, sizeof(struct ext4_dir_entry_2));
            found_dir = 1;
        }
        else
            free(dir_entries[i].name);
    }
    return !found_dir;
}

void print_bytes(u_char * bytes, uint64_t len, uint64_t index_offset)
{
    const static uint8_t ROW_LEN = 16;
    uint64_t i;
    for (i = 0; i < len; ++i)
    {
        if (i % ROW_LEN == 0)
//            printf("0x%08" PRIx64 "`0x%08" PRIx64 " ", ((index_offset + i) >> 32u ) << 32u, ((index_offset + i) << 32u ) >> 32u);
            printf("0x%08" PRIx64 "`%08" PRIx64 " ", (index_offset + i) & 0xFFFFFFFF00000000, (index_offset + i) & 0x00000000FFFFFFFF);

        printf("%02x ", (u_char)bytes[i]);
        if (i % ROW_LEN == ROW_LEN - 1)
            printf("\n");
    }
    if (i % ROW_LEN != ROW_LEN - 1)
        printf("\n");
}

void shell(FILE * file, struct SuperBlock * superBlock)
{
    static const uint MAX_INPUT_SIZE = 512;
    char buffer[MAX_INPUT_SIZE];
    static uint64_t root_inode_id = 2;  // 2 is always root directory
    uint64_t current_inode_id = root_inode_id;
    struct InodeTable current_directory;
    if (load_inode_table(file, superBlock, &current_directory, root_inode_id))
    {
        printf("Error loading root directory");
        return;
    }
    while (1)
    {
        printf("> ");
        fgets(buffer, MAX_INPUT_SIZE, stdin);
        if ((strlen(buffer) > 0) && (buffer[strlen (buffer) - 1] == '\n'))  // strip \n if there is one
            buffer[strlen (buffer) - 1] = '\0';

        if (strcmp(buffer, "cd") == 0)
        {
            if (shell_load_directory(file, superBlock, &current_directory, &current_inode_id, root_inode_id))
                return;
        }
        else if (strncmp(buffer, "cd ", 3) == 0)
        {
            struct ext4_dir_entry_2 * dir_entries;
            uint64_t elements_count;
            get_directory_list(file, superBlock, &current_directory, &dir_entries, &elements_count);
            uint8_t found_dir = 0;
            for (uint64_t i = 0; i < elements_count; ++i)
            {
                if (strncmp(buffer + 3, dir_entries[i].name, dir_entries[i].name_len) == 0)
                {
                    if (!(dir_entries[i].file_type & DEFT_DIRECTORY))
                        printf("%.*s is not a directory\n", dir_entries[i].name_len, dir_entries[i].name );
                    else if (shell_load_directory(file, superBlock, &current_directory, &current_inode_id, dir_entries[i].inode))
                        return;
                    found_dir = 1;
                }
                free(dir_entries[i].name);
            }
            free(dir_entries);
            if (!found_dir)
                printf("No such directory as \"%s\"\n", buffer + 3);
        }
        else if (strcmp(buffer, "ls") == 0)
        {
            struct ext4_dir_entry_2 * dir_entries;
            uint64_t elements_count;
            get_directory_list(file, superBlock, &current_directory, &dir_entries, &elements_count);
            for (uint64_t i = 0; i < elements_count; ++i)
            {
                char file_type;
                if (dir_entries[i].file_type & DEFT_REGULAR) file_type = 'f';
                else if (dir_entries[i].file_type & DEFT_DIRECTORY) file_type = 'd';
                else file_type = '?';
                printf("%c\t%.*s\t%" PRIu32 "\n", file_type, dir_entries[i].name_len, dir_entries[i].name, dir_entries[i].inode);
                free(dir_entries[i].name);
            }
            free(dir_entries);
        }
        else if (strcmp(buffer, "cat") == 0)
            printf("Please provide file name\n");
        else if (strncmp(buffer, "cat ", 4) == 0)
        {
            struct ext4_dir_entry_2 found_entry;
            if (find_path_in_directory(file, superBlock, &current_directory, buffer + 4, &found_entry))
            {
                printf("No such file as \"%s\"\n", buffer + 4);
                continue;
            }
            else if (!(found_entry.file_type & DEFT_REGULAR))
            {
                printf("\"%s\" is not a regular file\n", buffer + 4);
                continue;
            }
            struct InodeTable inode;
            if (load_inode_table(file, superBlock, &inode, found_entry.inode))
            {
                printf("Error loading inode %" PRIu32 "\n", found_entry.inode);
                continue;
            }
            uint64_t * blocks;
            uint64_t block_count;
            if (get_inode_block_list(file, superBlock, &inode, &blocks, &block_count))
            {
                printf("Error getting inode block list\n");
                continue;
            }
            uint64_t bytes_read = 0;
            uint64_t bytes_left_to_read = inode.i_size_u64;
            u_char * block_bytes = malloc(sizeof(u_char) * superBlock->s_block_size);
            for (uint64_t i = 0; i < block_count; ++i)
            {
                read_block(file, superBlock->s_block_size, blocks[i], (char *)block_bytes);
                uint64_t bytes_to_read = (bytes_left_to_read > superBlock->s_block_size) ? superBlock->s_block_size : bytes_left_to_read;
                printf("Page %" PRIu64 "\n", i);
                print_bytes(block_bytes, bytes_to_read, bytes_read);
                bytes_left_to_read -= bytes_to_read;
                bytes_read += bytes_to_read;
            }
            uint8_t a = 1;
        }
        else if (strcmp(buffer, "exit") == 0)
            break;
        else
            printf("error unknown command \"%s\"\n", buffer);
    }
    printf("Bye\n");
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
    load_super_block(file, &superBlock);

    if (superBlock.s_feature_incompat & REQUIRED_FEATURE_FLEX_BLOCK_GROUPS) printf("SYSTEM USES FLEX GROUPS\n");
    if (superBlock.s_feature_incompat & REQUIRED_FEATURE_64BIT) printf("SYSTEM USES 64BIT FEATURE\n");
    if (superBlock.s_feature_incompat & INCOMPAT_FILETYPE) printf("INCOMPAT_FILETYPE filesystem uses ext4_dir_entry_2\n");
    if (superBlock.s_feature_incompat & INCOMPAT_DIRDATA) printf("INCOMPAT_DIRDATA\n");

    shell(file, &superBlock);
    fclose(file);
    return 0;
}
