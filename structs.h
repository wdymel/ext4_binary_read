//
// Created by wdymel on 2020-11-11.
//

#ifndef PART_1_STRUTCS_H
#define PART_1_STRUTCS_H

#endif //PART_1_STRUTCS_H
#include <stdlib.h>

struct SuperBlock {  // numbers in the comments show bits that fields occupy in the superblock
    // standard fields
    u_int32_t inodes_count;  // 0x0
    u_int32_t _blocks_count_lo;  // 0x4 TODO split this variable into private lower and upper 32 bit values and one combined public 64 bit value
    u_int32_t _log_block_size;  // 0x18 Block size is 2 ^ (10 + s_log_block_size).
    u_int32_t blocks_per_group; // 0x20
    u_int32_t inodes_per_group; //0x28
    u_int16_t minor_version;  // 0x3e
    u_int32_t major_version;  // 0x4c

    // extended fields
    u_int32_t first_non_reserved_inode; // 0x54
    u_int16_t inode_size;  // 0x58
    u_int32_t required_features;  // 0x60

    // 64 bit features
    u_int16_t group_desc_size;  // 0xFE  size of group descriptors, in bytes, only valid if 64 bit flag is set
    u_int32_t _block_count_hi; // 0x150
    u_int8_t _log_groups_per_flex; // 0x174

    // manually calculated or combined values
    u_int64_t block_count;
    u_int32_t block_size;  // block size is 2 ^ (10 + s_log_block_size)
    u_int64_t groups_per_flex;  // size of a flexible block group is 2 ^ log_groups_per_flex.
    u_int64_t total_block_groups;  // has to be calculated manually, max theoretical size is 2^51
    u_int8_t group_desc_tab_block_addr;  // block address
};

int SuperBblock_new(struct SuperBlock * superblock, char * sb_bytes);

struct GroupDescriptor {
    u_int32_t bg_block_bitmap_lo;  // Lower 32-bits of location of block bitmap.
    u_int32_t bg_inode_bitmap_lo;  // Lower 32-bits of location of inode bitmap.
    u_int32_t bg_inode_table_lo;  // Lower 32-bits of location of inode table.
    u_int16_t bg_free_blocks_count_lo;  // Lower 16-bits of free block count.
    u_int16_t bg_free_inodes_count_lo;  // Lower 16-bits of free inode count.
    u_int16_t bg_used_dirs_count_lo;  // Lower 16-bits of directory count.
    u_int16_t bg_flags;  // Block group flags. Any of:
                         //0x1 	inode table and bitmap are not initialized (EXT4_BG_INODE_UNINIT).
                         //0x2 	block bitmap is not initialized (EXT4_BG_BLOCK_UNINIT).
                         //0x4 	inode table is zeroed (EXT4_BG_INODE_ZEROED).
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

struct InodeTable {  // TODO add 0x24, 0x28, and 0x74 manually
    u_int16_t i_mode;   // File mode. Any of:
                        //0x1 	S_IXOTH (Others may execute)
                        //0x2 	S_IWOTH (Others may write)
                        //0x4 	S_IROTH (Others may read)
                        //0x8 	S_IXGRP (Group members may execute)
                        //0x10 	S_IWGRP (Group members may write)
                        //0x20 	S_IRGRP (Group members may read)
                        //0x40 	S_IXUSR (Owner may execute)
                        //0x80 	S_IWUSR (Owner may write)
                        //0x100 	S_IRUSR (Owner may read)
                        //0x200 	S_ISVTX (Sticky bit)
                        //0x400 	S_ISGID (Set GID)
                        //0x800 	S_ISUID (Set UID)
                        //These are mutually-exclusive file types:
                        //0x1000 	S_IFIFO (FIFO)
                        //0x2000 	S_IFCHR (Character device)
                        //0x4000 	S_IFDIR (Directory)
                        //0x6000 	S_IFBLK (Block device)
                        //0x8000 	S_IFREG (Regular file)
                        //0xA000 	S_IFLNK (Symbolic link)
                        //0xC000 	S_IFSOCK (Socket)
    u_int16_t i_uid;  // Lower 16-bits of Owner UID.
    u_int32_t i_size_lo;  // Lower 32-bits of size in bytes.
    u_int32_t i_atime;  // Last access time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the checksum of the value.
    u_int32_t i_ctime;  // Last inode change time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the lower 32 bits of the attribute value's reference count.
    u_int32_t i_mtime;  // Last data modification time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the number of the inode that owns the extended attribute.
    u_int32_t i_dtime;  // Deletion Time, in seconds since the epoch.
    u_int16_t i_gid;  // Lower 16-bits of GID.
    u_int16_t i_links_count;  // Hard link count. Normally, ext4 does not permit an inode to have more than 65,000 hard links. This applies to files as well as directories, which means that there cannot be more than 64,998 subdirectories in a directory (each subdirectory's '..' entry counts as a hard link, as does the '.' entry in the directory itself). With the DIR_NLINK feature enabled, ext4 supports more than 64,998 subdirectories by setting this field to 1 to indicate that the number of hard links is not known.
    u_int32_t i_blocks_lo;  // Lower 32-bits of "block" count. If the huge_file feature flag is not set on the filesystem, the file consumes i_blocks_lo 512-byte blocks on disk. If huge_file is set and EXT4_HUGE_FILE_FL is NOT set in inode.i_flags, then the file consumes i_blocks_lo + (i_blocks_hi << 32) 512-byte blocks on disk. If huge_file is set and EXT4_HUGE_FILE_FL IS set in inode.i_flags, then this file consumes (i_blocks_lo + i_blocks_hi << 32) filesystem blocks on disk.
    u_int32_t i_flags;  // Inode flags. Any of:
                        //0x1 	This file requires secure deletion (EXT4_SECRM_FL). (not implemented)
                        //0x2 	This file should be preserved, should undeletion be desired (EXT4_UNRM_FL). (not implemented)
                        //0x4 	File is compressed (EXT4_COMPR_FL). (not really implemented)
                        //0x8 	All writes to the file must be synchronous (EXT4_SYNC_FL).
                        //0x10 	File is immutable (EXT4_IMMUTABLE_FL).
                        //0x20 	File can only be appended (EXT4_APPEND_FL).
                        //0x40 	The dump(1) utility should not dump this file (EXT4_NODUMP_FL).
                        //0x80 	Do not update access time (EXT4_NOATIME_FL).
                        //0x100 	Dirty compressed file (EXT4_DIRTY_FL). (not used)
                        //0x200 	File has one or more compressed clusters (EXT4_COMPRBLK_FL). (not used)
                        //0x400 	Do not compress file (EXT4_NOCOMPR_FL). (not used)
                        //0x800 	Encrypted inode (EXT4_ENCRYPT_FL). This bit value previously was EXT4_ECOMPR_FL (compression error), which was never used.
                        //0x1000 	Directory has hashed indexes (EXT4_INDEX_FL).
                        //0x2000 	AFS magic directory (EXT4_IMAGIC_FL).
                        //0x4000 	File data must always be written through the journal (EXT4_JOURNAL_DATA_FL).
                        //0x8000 	File tail should not be merged (EXT4_NOTAIL_FL). (not used by ext4)
                        //0x10000 	All directory entry data should be written synchronously (see dirsync) (EXT4_DIRSYNC_FL).
                        //0x20000 	Top of directory hierarchy (EXT4_TOPDIR_FL).
                        //0x40000 	This is a huge file (EXT4_HUGE_FILE_FL).
                        //0x80000 	Inode uses extents (EXT4_EXTENTS_FL).
                        //0x200000 	Inode stores a large extended attribute value in its data blocks (EXT4_EA_INODE_FL).
                        //0x400000 	This file has blocks allocated past EOF (EXT4_EOFBLOCKS_FL). (deprecated)
                        //0x01000000 	Inode is a snapshot (EXT4_SNAPFILE_FL). (not in mainline)
                        //0x04000000 	Snapshot is being deleted (EXT4_SNAPFILE_DELETED_FL). (not in mainline)
                        //0x08000000 	Snapshot shrink has completed (EXT4_SNAPFILE_SHRUNK_FL). (not in mainline)
                        //0x10000000 	Inode has inline data (EXT4_INLINE_DATA_FL).
                        //0x20000000 	Create children with the same project ID (EXT4_PROJINHERIT_FL).
                        //0x80000000 	Reserved for ext4 library (EXT4_RESERVED_FL).
                        //Aggregate flags:
                        //0x4BDFFF 	User-visible flags.
                        //0x4B80FF 	User-modifiable flags. Note that while EXT4_JOURNAL_DATA_FL and EXT4_EXTENTS_FL can be set with setattr, they are not in the kernel's EXT4_FL_USER_MODIFIABLE mask, since it needs to handle the setting of these flags in a special manner and they are masked out of the set of flags that are saved directly to i_flags.
    u_int32_t i_generation;  // File version (for NFS).
    u_int32_t i_file_acl_lo;  // Lower 32-bits of extended attribute block. ACLs are of course one of many possible extended attributes; I think the name of this field is a result of the first use of extended attributes being for ACLs.
    u_int32_t i_size_high;  // Upper 32-bits of file/directory size. In ext2/3 this field was named i_dir_acl, though it was usually set to zero and never used.
    u_int32_t i_obso_faddr;  // (Obsolete) fragment address.
    u_int16_t i_extra_isize;  // Size of this inode - 128. Alternately, the size of the extended inode fields beyond the original ext2 inode, including this field.
    u_int16_t i_checksum_hi;  // Upper 16-bits of the inode checksum.
    u_int32_t i_ctime_extra;  // Extra change time bits. This provides sub-second precision. See Inode Timestamps section.
    u_int32_t i_mtime_extra;  // Extra modification time bits. This provides sub-second precision.
    u_int32_t i_atime_extra;  // Extra access time bits. This provides sub-second precision.
    u_int32_t i_crtime;  // File creation time, in seconds since the epoch.
    u_int32_t i_crtime_extra;  // Extra file creation time bits. This provides sub-second precision.
    u_int32_t i_version_hi;  // Upper 32-bits for version number.
    u_int32_t i_projid;  // Project ID.
};

int InodeTable_new(struct InodeTable * inodeTable, char * sb_bytes);