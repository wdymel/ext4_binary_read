//
// Created by wdymel on 2020-12-19.
//

#ifndef EXT4_BINARY_READ_GROUP_DESCRIPTOR_H
#define EXT4_BINARY_READ_GROUP_DESCRIPTOR_H
#include <stdlib.h>
struct GroupDescriptor {
    u_int32_t bg_block_bitmap_lo;  // Lower 32-bits of location of block bitmap.
    u_int32_t bg_inode_bitmap_lo;  // Lower 32-bits of location of inode bitmap.
    u_int32_t bg_inode_table_lo;  // Lower 32-bits of location of inode table.
    u_int16_t bg_free_blocks_count_lo;  // Lower 16-bits of free block count.
    u_int16_t bg_free_inodes_count_lo;  // Lower 16-bits of free inode count.
    u_int16_t bg_used_dirs_count_lo;  // Lower 16-bits of directory count.
    // Block group flags. Any of:
    //0x1 	inode table and bitmap are not initialized (EXT4_BG_INODE_UNINIT).
    //0x2 	block bitmap is not initialized (EXT4_BG_BLOCK_UNINIT).
    //0x4 	inode table is zeroed (EXT4_BG_INODE_ZEROED).
    u_int16_t bg_flags;
    u_int32_t bg_exclude_bitmap_lo;  // Lower 32-bits of location of snapshot exclusion bitmap.
    u_int16_t bg_block_bitmap_csum_lo;  // Lower 16-bits of the block bitmap checksum.
    u_int16_t bg_inode_bitmap_csum_lo;  // Lower 16-bits of the inode bitmap checksum.
    u_int16_t bg_itable_unused_lo;  // Lower 16-bits of unused inode count. If set, we needn't scan past the (sb.s_inodes_per_group - gdt.bg_itable_unused)th entry in the inode table for this group.
    u_int16_t bg_checksum;  // Group descriptor checksum; crc16(sb_uuid+group+desc) if the RO_COMPAT_GDT_CSUM feature is set, or crc32c(sb_uuid+group_desc) & 0xFFFF if the RO_COMPAT_METADATA_CSUM feature is set.

    // These fields only exist if the 64bit feature is enabled and s_desc_size > 32.
    u_int32_t bg_block_bitmap_hi;  // Upper 32-bits of location of block bitmap.
    u_int32_t bg_inode_bitmap_hi;  // Upper 32-bits of location of inodes bitmap.
    u_int32_t bg_inode_table_hi;  // Upper 32-bits of location of inodes table.
    u_int16_t bg_free_blocks_count_hi;  // Upper 16-bits of free block count.
    u_int16_t bg_free_inodes_count_hi;  // Upper 16-bits of free inode count.
    u_int16_t bg_used_dirs_count_hi;  // Upper 16-bits of directory count.
    u_int16_t bg_itable_unused_hi;  // Upper 16-bits of unused inode count.
    u_int32_t bg_exclude_bitmap_hi;  // Upper 32-bits of location of snapshot exclusion bitmap.
    u_int16_t bg_block_bitmap_csum_hi;  // Upper 16-bits of the block bitmap checksum.
    u_int16_t bg_inode_bitmap_csum_hi;  // Upper 16-bits of the inode bitmap checksum.

    // These are combined hi and lo values
    u_int64_t bg_block_bitmap_u64;  // location of block bitmap.
    u_int64_t bg_inode_bitmap_u64;  // location of inode bitmap.
    u_int64_t bg_inode_table_u64;  // location of inode table.
    u_int32_t bg_free_blocks_count_u32;  // free block count.
    u_int32_t bg_free_inodes_count_u32;  // free inode count.
    u_int32_t bg_used_dirs_count_u32;  // directory count.
    u_int64_t bg_exclude_bitmap_u64;  // location of snapshot exclusion bitmap.
    u_int32_t bg_block_bitmap_csum_u32;  // block bitmap checksum.
    u_int32_t bg_inode_bitmap_csum_u32;  // inode bitmap checksum.
};

int GroupDescriptor_new(struct GroupDescriptor * groupDescriptor, char * sb_bytes, u_int16_t s_desc_size);
#endif //EXT4_BINARY_READ_GROUP_DESCRIPTOR_H
