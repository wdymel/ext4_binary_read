//
// Created by wdymel on 2020-12-19.
//

#include "group_descriptor.h"
#include "../interfaces.h"
int GroupDescriptor_new(struct GroupDescriptor *groupDescriptor, char *sb_bytes, u_int16_t s_desc_size) {
    /* Initializes an instance of GroupDescriptor from raw bytes data
     * groupDescriptor => pointer to a groupDescriptor instance
     * sb_bytes => pointer to 32 B or 64 B char array containing Group Descriptor data
     * */

    // standard values
    // Lower 32-bits of location of block bitmap.
    groupDescriptor->bg_block_bitmap_lo = convert_le_byte_array_to_uint(sb_bytes + 0x0,
                                                                        sizeof(groupDescriptor->bg_block_bitmap_lo));
    // Lower 32-bits of location of inode bitmap.
    groupDescriptor->bg_inode_bitmap_lo = convert_le_byte_array_to_uint(sb_bytes + 0x4,
                                                                        sizeof(groupDescriptor->bg_inode_bitmap_lo));
    // Lower 32-bits of location of inode table.
    groupDescriptor->bg_inode_table_lo = convert_le_byte_array_to_uint(sb_bytes + 0x8,
                                                                       sizeof(groupDescriptor->bg_inode_table_lo));
    // Lower 16-bits of free block count.
    groupDescriptor->bg_free_blocks_count_lo = convert_le_byte_array_to_uint(sb_bytes + 0xC,
                                                                             sizeof(groupDescriptor->bg_free_blocks_count_lo));
    // Lower 16-bits of free inode count.
    groupDescriptor->bg_free_inodes_count_lo = convert_le_byte_array_to_uint(sb_bytes + 0xE,
                                                                             sizeof(groupDescriptor->bg_free_inodes_count_lo));
    // Lower 16-bits of directory count.
    groupDescriptor->bg_used_dirs_count_lo = convert_le_byte_array_to_uint(sb_bytes + 0x10,
                                                                           sizeof(groupDescriptor->bg_used_dirs_count_lo));
    // Block group flags. Any of:
    //0x1 	inode table and bitmap are not initialized (EXT4_BG_INODE_UNINIT).
    //0x2 	block bitmap is not initialized (EXT4_BG_BLOCK_UNINIT).
    //0x4 	inode table is zeroed (EXT4_BG_INODE_ZEROED).
    groupDescriptor->bg_flags = convert_le_byte_array_to_uint(sb_bytes + 0x12, sizeof(groupDescriptor->bg_flags));

    // Lower 32-bits of location of snapshot exclusion bitmap.
    groupDescriptor->bg_exclude_bitmap_lo = convert_le_byte_array_to_uint(sb_bytes + 0x14,
                                                                          sizeof(groupDescriptor->bg_exclude_bitmap_lo));
    // Lower 16-bits of the block bitmap checksum.
    groupDescriptor->bg_block_bitmap_csum_lo = convert_le_byte_array_to_uint(sb_bytes + 0x18,
                                                                             sizeof(groupDescriptor->bg_block_bitmap_csum_lo));
    // Lower 16-bits of the inode bitmap checksum.
    groupDescriptor->bg_inode_bitmap_csum_lo = convert_le_byte_array_to_uint(sb_bytes + 0x1A,
                                                                             sizeof(groupDescriptor->bg_inode_bitmap_csum_lo));
    // Lower 16-bits of unused inode count. If set, we needn't scan past the (sb.s_inodes_per_group - gdt.bg_itable_unused)th entry in the inode table for this group.
    groupDescriptor->bg_itable_unused_lo = convert_le_byte_array_to_uint(sb_bytes + 0x1C,
                                                                         sizeof(groupDescriptor->bg_itable_unused_lo));
    // Group descriptor checksum; crc16(sb_uuid+group+desc) if the RO_COMPAT_GDT_CSUM feature is set, or crc32c(sb_uuid+group_desc) & 0xFFFF if the RO_COMPAT_METADATA_CSUM feature is set.
    groupDescriptor->bg_checksum = convert_le_byte_array_to_uint(sb_bytes + 0x1E, sizeof(groupDescriptor->bg_checksum));
    //These fields only exist if the 64bit feature is enabled and s_desc_size > 32.
    if (s_desc_size > 32)
    {
        // Upper 32-bits of location of block bitmap.
        groupDescriptor->bg_block_bitmap_hi = convert_le_byte_array_to_uint(sb_bytes + 0x20,
                                                                            sizeof(groupDescriptor->bg_block_bitmap_hi));
        // Upper 32-bits of location of inodes bitmap.
        groupDescriptor->bg_inode_bitmap_hi = convert_le_byte_array_to_uint(sb_bytes + 0x24,
                                                                            sizeof(groupDescriptor->bg_inode_bitmap_hi));
        // Upper 32-bits of location of inodes table.
        groupDescriptor->bg_inode_table_hi = convert_le_byte_array_to_uint(sb_bytes + 0x28,
                                                                           sizeof(groupDescriptor->bg_inode_table_hi));
        // Upper 16-bits of free block count.
        groupDescriptor->bg_free_blocks_count_hi = convert_le_byte_array_to_uint(sb_bytes + 0x2C,
                                                                                 sizeof(groupDescriptor->bg_free_blocks_count_hi));
        // Upper 16-bits of free inode count.
        groupDescriptor->bg_free_inodes_count_hi = convert_le_byte_array_to_uint(sb_bytes + 0x2E,
                                                                                 sizeof(groupDescriptor->bg_free_inodes_count_hi));
        // Upper 16-bits of directory count.
        groupDescriptor->bg_used_dirs_count_hi = convert_le_byte_array_to_uint(sb_bytes + 0x30,
                                                                               sizeof(groupDescriptor->bg_used_dirs_count_hi));
        // Upper 16-bits of unused inode count.
        groupDescriptor->bg_itable_unused_hi = convert_le_byte_array_to_uint(sb_bytes + 0x32,
                                                                             sizeof(groupDescriptor->bg_itable_unused_hi));
        // Upper 32-bits of location of snapshot exclusion bitmap.
        groupDescriptor->bg_exclude_bitmap_hi = convert_le_byte_array_to_uint(sb_bytes + 0x34,
                                                                              sizeof(groupDescriptor->bg_exclude_bitmap_hi));
        // Upper 16-bits of the block bitmap checksum.
        groupDescriptor->bg_block_bitmap_csum_hi = convert_le_byte_array_to_uint(sb_bytes + 0x38,
                                                                                 sizeof(groupDescriptor->bg_block_bitmap_csum_hi));
        // Upper 16-bits of the inode bitmap checksum.
        groupDescriptor->bg_inode_bitmap_csum_hi = convert_le_byte_array_to_uint(sb_bytes + 0x3A,
                                                                                 sizeof(groupDescriptor->bg_inode_bitmap_csum_hi));

        groupDescriptor->bg_block_bitmap_u64 = ((u_int64_t)groupDescriptor->bg_block_bitmap_hi << 32u) + groupDescriptor->bg_block_bitmap_lo;
        groupDescriptor->bg_inode_bitmap_u64 = ((u_int64_t)groupDescriptor->bg_inode_bitmap_hi << 32u) + groupDescriptor->bg_inode_bitmap_lo;
        groupDescriptor->bg_inode_table_u64 = ((u_int64_t)groupDescriptor->bg_inode_table_hi << 32u) + groupDescriptor->bg_inode_table_lo;
        groupDescriptor->bg_free_blocks_count_u32 = ((u_int32_t)groupDescriptor->bg_free_blocks_count_hi << 16u) + groupDescriptor->bg_free_blocks_count_lo;
        groupDescriptor->bg_free_inodes_count_u32 = ((u_int32_t)groupDescriptor->bg_free_inodes_count_hi << 16u) + groupDescriptor->bg_free_inodes_count_lo;
        groupDescriptor->bg_used_dirs_count_u32 = ((u_int32_t)groupDescriptor->bg_used_dirs_count_hi << 16u) + groupDescriptor->bg_used_dirs_count_lo;
        groupDescriptor->bg_exclude_bitmap_u64 = ((u_int64_t)groupDescriptor->bg_exclude_bitmap_hi << 32u) + groupDescriptor->bg_exclude_bitmap_lo;
        groupDescriptor->bg_block_bitmap_csum_u32 = ((u_int32_t)groupDescriptor->bg_block_bitmap_csum_hi << 16u) + groupDescriptor->bg_block_bitmap_csum_lo;
        groupDescriptor->bg_inode_bitmap_csum_u32 = ((u_int32_t)groupDescriptor->bg_inode_bitmap_csum_hi << 16u) + groupDescriptor->bg_inode_bitmap_csum_lo;
    } else
    {
        groupDescriptor->bg_block_bitmap_u64 = groupDescriptor->bg_block_bitmap_lo;
        groupDescriptor->bg_inode_bitmap_u64 = groupDescriptor->bg_inode_bitmap_lo;
        groupDescriptor->bg_inode_table_u64 = groupDescriptor->bg_inode_table_lo;
        groupDescriptor->bg_free_blocks_count_u32 = groupDescriptor->bg_free_blocks_count_lo;
        groupDescriptor->bg_free_inodes_count_u32 = groupDescriptor->bg_free_inodes_count_lo;
        groupDescriptor->bg_used_dirs_count_u32 = groupDescriptor->bg_used_dirs_count_lo;
        groupDescriptor->bg_exclude_bitmap_u64 = groupDescriptor->bg_exclude_bitmap_lo;
        groupDescriptor->bg_block_bitmap_csum_u32 = groupDescriptor->bg_block_bitmap_csum_lo;
        groupDescriptor->bg_inode_bitmap_csum_u32 = groupDescriptor->bg_inode_bitmap_csum_lo;
    }

    return 0;
}