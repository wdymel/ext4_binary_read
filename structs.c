//
// Created by wdymel on 2020-11-11.
//
#include <math.h>
#include "structs.h"
#include "interfaces.h"
#include "flags.h"

int SuperBblock_new(struct SuperBlock *superblock, char *sb_bytes) {
    superblock->inodes_count = convert_le_byte_array_to_uint(sb_bytes + 0x0, sizeof(superblock->inodes_count));
    superblock->_blocks_count_lo = convert_le_byte_array_to_uint(sb_bytes + 0x4, sizeof(superblock->_blocks_count_lo));
    superblock->block_count = superblock->_blocks_count_lo;  // block count can be extended to 64 bits if 64 bit flag is set, we check for it later in the code
    superblock->_log_block_size = convert_le_byte_array_to_uint(sb_bytes + 0x18, sizeof(superblock->_log_block_size));
    superblock->block_size = 1024u << superblock->_log_block_size;  // block size is 2 ^ (10 + s_log_block_size)
    superblock->blocks_per_group = convert_le_byte_array_to_uint(sb_bytes + 0x20, sizeof(superblock->blocks_per_group));
    superblock->inodes_per_group = convert_le_byte_array_to_uint(sb_bytes + 0x28, sizeof(superblock->inodes_per_group));
    superblock->minor_version = convert_le_byte_array_to_uint(sb_bytes + 0x3e, sizeof(superblock->minor_version));
    superblock->major_version = convert_le_byte_array_to_uint(sb_bytes + 0x4c, sizeof(superblock->major_version));
    // if major version is >= 1 then read data from extended fields
    if (superblock->major_version >= 1) {
        superblock->first_non_reserved_inode = convert_le_byte_array_to_uint(sb_bytes + 0x54,
                                                                             sizeof(superblock->first_non_reserved_inode));
        superblock->inode_size = convert_le_byte_array_to_uint(sb_bytes + 0x58, sizeof(superblock->inode_size));
        superblock->required_features = convert_le_byte_array_to_uint(sb_bytes + 0x60,
                                                                      sizeof(superblock->required_features));
    } else // for major version < 1 use defaults
    {
        superblock->first_non_reserved_inode = 11;
        superblock->inode_size = 128;
        superblock->required_features = 0;
    }
    // 64 bit features
    if (superblock->required_features & REQUIRED_FEATURE_64BIT) {
        superblock->group_desc_size = convert_le_byte_array_to_uint(sb_bytes + 0xfe,
                                                                    sizeof(superblock->group_desc_size));
        superblock->_block_count_hi = convert_le_byte_array_to_uint(sb_bytes + 0x150,
                                                                    sizeof(superblock->_block_count_hi));
        superblock->block_count = ((u_int64_t) superblock->_block_count_hi << 32u) + superblock->_blocks_count_lo;
        superblock->_log_groups_per_flex = convert_le_byte_array_to_uint(sb_bytes + 0x174,
                                                                         sizeof(superblock->_log_groups_per_flex));
        superblock->groups_per_flex = (u_int64_t) 1u << superblock->_log_groups_per_flex;
    }
    // calculate number of block groups https://wiki.osdev.org/Ext2#Determining_the_Number_of_Block_Groups
    {
        unsigned method1, method2;
        method1 = ceil((double) superblock->_blocks_count_lo / superblock->blocks_per_group);
        method2 = ceil((double) superblock->inodes_count / superblock->inodes_per_group);
        if (method1 != method2) fprintf(stderr, "Values for number of block groups do not match up");
        superblock->total_block_groups = method1 < method2 ? method2 : method1;
    }
    // locating the block group descriptor table https://wiki.osdev.org/Ext2#Locating_the_Block_Group_Descriptor_Table
    superblock->group_desc_tab_block_addr = superblock->block_size == 1024 ? 2 : 1;
    return 0;
}

int GroupDescriptor_new(struct GroupDescriptor *groupDescriptor, char *sb_bytes, u_int16_t s_desc_size) {
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

int InodeTable_new(struct InodeTable * inodeTable, char * sb_bytes)
{
    // File mode. Any of:
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
    inodeTable->i_mode = convert_le_byte_array_to_uint(sb_bytes + 0x0, sizeof(inodeTable->i_mode));
    // Lower 16-bits of Owner UID.
    inodeTable->i_uid = convert_le_byte_array_to_uint(sb_bytes + 0x2, sizeof(inodeTable->i_uid));
    // Lower 32-bits of size in bytes.
    inodeTable->i_size_lo = convert_le_byte_array_to_uint(sb_bytes + 0x4, sizeof(inodeTable->i_size_lo));
    // Last access time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the checksum of the value.
    inodeTable->i_atime = convert_le_byte_array_to_uint(sb_bytes + 0x8, sizeof(inodeTable->i_atime));
    // Last inode change time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the lower 32 bits of the attribute value's reference count.
    inodeTable->i_ctime = convert_le_byte_array_to_uint(sb_bytes + 0xC, sizeof(inodeTable->i_ctime));
    // Last data modification time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the number of the inode that owns the extended attribute.
    inodeTable->i_mtime = convert_le_byte_array_to_uint(sb_bytes + 0x10, sizeof(inodeTable->i_mtime));
    // Deletion Time, in seconds since the epoch.
    inodeTable->i_dtime = convert_le_byte_array_to_uint(sb_bytes + 0x14, sizeof(inodeTable->i_dtime));
    // Lower 16-bits of GID.
    inodeTable->i_gid = convert_le_byte_array_to_uint(sb_bytes + 0x18, sizeof(inodeTable->i_gid));
    // Hard link count. Normally, ext4 does not permit an inode to have more than 65,000 hard links. This applies to files as well as directories, which means that there cannot be more than 64,998 subdirectories in a directory (each subdirectory's '..' entry counts as a hard link, as does the '.' entry in the directory itself). With the DIR_NLINK feature enabled, ext4 supports more than 64,998 subdirectories by setting this field to 1 to indicate that the number of hard links is not known.
    inodeTable->i_links_count = convert_le_byte_array_to_uint(sb_bytes + 0x1A, sizeof(inodeTable->i_links_count));
    // Lower 32-bits of "block" count. If the huge_file feature flag is not set on the filesystem, the file consumes i_blocks_lo 512-byte blocks on disk. If huge_file is set and EXT4_HUGE_FILE_FL is NOT set in inode.i_flags, then the file consumes i_blocks_lo + (i_blocks_hi << 32) 512-byte blocks on disk. If huge_file is set and EXT4_HUGE_FILE_FL IS set in inode.i_flags, then this file consumes (i_blocks_lo + i_blocks_hi << 32) filesystem blocks on disk.
    inodeTable->i_blocks_lo = convert_le_byte_array_to_uint(sb_bytes + 0x1C, sizeof(inodeTable->i_blocks_lo));
    // Inode flags. Any of:
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
    inodeTable->i_flags = convert_le_byte_array_to_uint(sb_bytes + 0x20, sizeof(inodeTable->i_flags));
    // File version (for NFS).
    inodeTable->i_generation = convert_le_byte_array_to_uint(sb_bytes + 0x64, sizeof(inodeTable->i_generation));
    // Lower 32-bits of extended attribute block. ACLs are of course one of many possible extended attributes; I think the name of this field is a result of the first use of extended attributes being for ACLs.
    inodeTable->i_file_acl_lo = convert_le_byte_array_to_uint(sb_bytes + 0x68, sizeof(inodeTable->i_file_acl_lo));
    // Upper 32-bits of file/directory size. In ext2/3 this field was named i_dir_acl, though it was usually set to zero and never used.
    inodeTable->i_size_high= convert_le_byte_array_to_uint(sb_bytes + 0x6C, sizeof(inodeTable->i_size_high));
    // (Obsolete) fragment address.
    inodeTable->i_obso_faddr = convert_le_byte_array_to_uint(sb_bytes + 0x70, sizeof(inodeTable->i_obso_faddr));
    // Size of this inode - 128. Alternately, the size of the extended inode fields beyond the original ext2 inode, including this field.
    inodeTable->i_extra_isize = convert_le_byte_array_to_uint(sb_bytes + 0x80, sizeof(inodeTable->i_extra_isize));
    // Upper 16-bits of the inode checksum.
    inodeTable->i_checksum_hi = convert_le_byte_array_to_uint(sb_bytes + 0x82, sizeof(inodeTable->i_checksum_hi));
    // Extra change time bits. This provides sub-second precision. See Inode Timestamps section.
    inodeTable->i_ctime_extra = convert_le_byte_array_to_uint(sb_bytes + 0x84, sizeof(inodeTable->i_ctime_extra));
    // Extra modification time bits. This provides sub-second precision.
    inodeTable->i_mtime_extra = convert_le_byte_array_to_uint(sb_bytes + 0x88, sizeof(inodeTable->i_mtime_extra));
    // Extra access time bits. This provides sub-second precision.
    inodeTable->i_atime_extra = convert_le_byte_array_to_uint(sb_bytes + 0x8C, sizeof(inodeTable->i_atime_extra));
    // File creation time, in seconds since the epoch.
    inodeTable->i_crtime = convert_le_byte_array_to_uint(sb_bytes + 0x90, sizeof(inodeTable->i_crtime));
    // Extra file creation time bits. This provides sub-second precision.
    inodeTable->i_crtime_extra = convert_le_byte_array_to_uint(sb_bytes + 0x94, sizeof(inodeTable->i_crtime_extra));
    // Upper 32-bits for version number.
    inodeTable->i_version_hi = convert_le_byte_array_to_uint(sb_bytes + 0x98, sizeof(inodeTable->i_version_hi));
    // Project ID.
    inodeTable->i_projid = convert_le_byte_array_to_uint(sb_bytes + 0x9C, sizeof(inodeTable->i_projid));
}