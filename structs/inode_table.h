//
// Created by wdymel on 2020-12-19.
//

#ifndef EXT4_BINARY_READ_INODE_TABLE_H
#define EXT4_BINARY_READ_INODE_TABLE_H
#include <stdlib.h>

// region inode constants
// region u_int16_t i_mode
// File mode. Any of:

static const u_int16_t S_IXOTH = 0x1; // (Others may execute)
static const u_int16_t S_IWOTH = 0x2; // (Others may write)
static const u_int16_t S_IROTH = 0x4; // (Others may read)
static const u_int16_t S_IXGRP = 0x8; // (Group members may execute)
static const u_int16_t S_IWGRP = 0x10; // (Group members may write)
static const u_int16_t S_IRGRP = 0x20; // (Group members may read)
static const u_int16_t S_IXUSR = 0x40; // (Owner may execute)
static const u_int16_t S_IWUSR = 0x80; // (Owner may write)
static const u_int16_t S_IRUSR = 0x100; // (Owner may read)
static const u_int16_t S_ISVTX = 0x200; // (Sticky bit)
static const u_int16_t S_ISGID = 0x400; // (Set GID)
static const u_int16_t S_ISUID = 0x800; // (Set UID)
//These are mutually-exclusive file types:

static const u_int16_t S_IFIFO = 0x1000; // (FIFO)
static const u_int16_t S_IFCHR = 0x2000; // (Character device)
static const u_int16_t S_IFDIR = 0x4000; // (Directory)
static const u_int16_t S_IFBLK = 0x6000; // (Block device)
static const u_int16_t S_IFREG = 0x8000; // (Regular file)
static const u_int16_t S_IFLNK = 0xA000; // (Symbolic link)
static const u_int16_t S_IFSOCK = 0xC000; // (Socket)
// endregion u_int16_t i_mode flags

// region u_int_32_t i_flags flags
// Inode flags. Any of:

static const u_int32_t EXT4_SECRM_FL = 0x1; // This file requires secure deletion (EXT4_SECRM_FL). (not implemented)
static const u_int32_t EXT4_UNRM_FL = 0x2; // This file should be preserved, should undeletion be desired (EXT4_UNRM_FL). (not implemented)
static const u_int32_t EXT4_COMPR_FL = 0x4; // File is compressed (EXT4_COMPR_FL). (not really implemented)
static const u_int32_t EXT4_SYNC_FL = 0x8; // All writes to the file must be synchronous (EXT4_SYNC_FL).
static const u_int32_t EXT4_IMMUTABLE_FL = 0x10; // File is immutable (EXT4_IMMUTABLE_FL).
static const u_int32_t EXT4_APPEND_FL = 0x20; // File can only be appended (EXT4_APPEND_FL).
static const u_int32_t EXT4_NODUMP_FL = 0x40; // The dump(1) utility should not dump this file (EXT4_NODUMP_FL).
static const u_int32_t EXT4_NOATIME_FL = 0x80; // Do not update access time (EXT4_NOATIME_FL).
static const u_int32_t EXT4_DIRTY_FL = 0x100; // Dirty compressed file (EXT4_DIRTY_FL). (not used)
static const u_int32_t EXT4_COMPRBLK_FL = 0x200; // File has one or more compressed clusters (EXT4_COMPRBLK_FL). (not used)
static const u_int32_t EXT4_NOCOMPR_FL = 0x400; // Do not compress file (EXT4_NOCOMPR_FL). (not used)
static const u_int32_t EXT4_ENCRYPT_FL = 0x800; // Encrypted inode (EXT4_ENCRYPT_FL). This bit value previously was EXT4_ECOMPR_FL (compression error), which was never used.
static const u_int32_t EXT4_INDEX_FL = 0x1000; // Directory has hashed indexes (EXT4_INDEX_FL).
static const u_int32_t EXT4_IMAGIC_FL = 0x2000; // AFS magic directory (EXT4_IMAGIC_FL).
static const u_int32_t EXT4_JOURNAL_DATA_FL = 0x4000; // File data must always be written through the journal (EXT4_JOURNAL_DATA_FL).
static const u_int32_t EXT4_NOTAIL_FL = 0x8000; // File tail should not be merged (EXT4_NOTAIL_FL). (not used by ext4)
static const u_int32_t EXT4_DIRSYNC_FL = 0x10000; // All directory entry data should be written synchronously (see dirsync) (EXT4_DIRSYNC_FL).
static const u_int32_t EXT4_TOPDIR_FL = 0x20000; // Top of directory hierarchy (EXT4_TOPDIR_FL).
static const u_int32_t EXT4_HUGE_FILE_FL = 0x40000; // This is a huge file (EXT4_HUGE_FILE_FL).
static const u_int32_t EXT4_EXTENTS_FL = 0x80000; // Inode uses extents (EXT4_EXTENTS_FL).
static const u_int32_t EXT4_EA_INODE_FL = 0x200000; // Inode stores a large extended attribute value in its data blocks (EXT4_EA_INODE_FL).
static const u_int32_t EXT4_EOFBLOCKS_FL = 0x400000; // This file has blocks allocated past EOF (EXT4_EOFBLOCKS_FL). (deprecated)
static const u_int32_t EXT4_SNAPFILE_FL = 0x01000000; // Inode is a snapshot (EXT4_SNAPFILE_FL). (not in mainline)
static const u_int32_t EXT4_SNAPFILE_DELETED_FL = 0x04000000; // Snapshot is being deleted (EXT4_SNAPFILE_DELETED_FL). (not in mainline)
static const u_int32_t EXT4_SNAPFILE_SHRUNK_FL = 0x08000000; // Snapshot shrink has completed (EXT4_SNAPFILE_SHRUNK_FL). (not in mainline)
static const u_int32_t EXT4_INLINE_DATA_FL = 0x10000000; // Inode has inline data (EXT4_INLINE_DATA_FL).
static const u_int32_t EXT4_PROJINHERIT_FL = 0x20000000; // Create children with the same project ID (EXT4_PROJINHERIT_FL).
static const u_int32_t EXT4_RESERVED_FL = 0x80000000; // Reserved for ext4 library (EXT4_RESERVED_FL).
//Aggregate flags:
//0x4BDFFF 	User-visible flags.
//0x4B80FF 	User-modifiable flags. Note that while EXT4_JOURNAL_DATA_FL and EXT4_EXTENTS_FL can be set with setattr, they are not in the kernel's EXT4_FL_USER_MODIFIABLE mask, since it needs to handle the setting of these flags in a special manner and they are masked out of the set of flags that are saved directly to i_flags.
// endregion u_int_32_t i_flags flags
// endregion inode constants


struct InodeTable {  // TODO add 0x24, 0x28, and 0x74 manually
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
    u_int16_t i_mode;
    u_int16_t i_uid;  // Lower 16-bits of Owner UID.
    u_int32_t i_size_lo;  // Lower 32-bits of size in bytes.
    u_int32_t i_atime;  // Last access time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the checksum of the value.
    u_int32_t i_ctime;  // Last inode change time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the lower 32 bits of the attribute value's reference count.
    u_int32_t i_mtime;  // Last data modification time, in seconds since the epoch. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the number of the inode that owns the extended attribute.
    u_int32_t i_dtime;  // Deletion Time, in seconds since the epoch.
    u_int16_t i_gid;  // Lower 16-bits of GID.
    u_int16_t i_links_count;  // Hard link count. Normally, ext4 does not permit an inode to have more than 65,000 hard links. This applies to files as well as directories, which means that there cannot be more than 64,998 subdirectories in a directory (each subdirectory's '..' entry counts as a hard link, as does the '.' entry in the directory itself). With the DIR_NLINK feature enabled, ext4 supports more than 64,998 subdirectories by setting this field to 1 to indicate that the number of hard links is not known.
    u_int32_t i_blocks_lo;  // Lower 32-bits of "block" count. If the huge_file feature flag is not set on the filesystem, the file consumes i_blocks_lo 512-byte blocks on disk. If huge_file is set and EXT4_HUGE_FILE_FL is NOT set in inode.i_flags, then the file consumes i_blocks_lo + (i_blocks_hi << 32) 512-byte blocks on disk. If huge_file is set and EXT4_HUGE_FILE_FL IS set in inode.i_flags, then this file consumes (i_blocks_lo + i_blocks_hi << 32) filesystem blocks on disk.
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
    u_int32_t i_flags;
    u_int32_t l_i_version;  // Inode version. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the upper 32 bits of the attribute value's reference count.
    u_char i_block[60];  // Block map or extent tree. See the section "The Contents of inode.i_block".
    u_int16_t l_i_blocks_high;  // Upper 16-bits of the block count. Please see the note attached to i_blocks_lo.
    u_int16_t l_i_file_acl_high;  // Upper 16-bits of the extended attribute block (historically, the file ACL location). See the Extended Attributes section below.
    u_int16_t l_i_uid_high;  // Upper 16-bits of the Owner UID.
    u_int16_t l_i_gid_high;  // Upper 16-bits of the GID.
    u_int16_t l_i_checksum_lo;  // Lower 16-bits of the inode checksum.
    u_int16_t l_i_reserved;  // Unused.
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

struct ext4_extent_header {
    u_int16_t eh_magic;  // Magic number, 0xF30A.
    u_int16_t eh_entries; // Number of valid entries following the header.
    u_int16_t eh_max;  // Maximum number of entries that could follow the header.
    u_int16_t eh_depth;  // Depth of this extent node in the extent tree. 0 = this extent node points to data blocks; otherwise, this extent node points to other extent nodes. The extent tree can be at most 5 levels deep: a logical block number can be at most 2^32, and the smallest n that satisfies 4*(((blocksize - 12)/12)^n) >= 2^32 is 5.
    u_int32_t eh_generation;  // Generation of the tree. (Used by Lustre, but not standard ext4).
};
int ext4_extent_header_new(struct ext4_extent_header * ext4_extent_header, char * bytes);

struct ext4_extent_idx {
    u_int32_t ei_block;  // This index node covers file blocks from 'block' onward.
    u_int32_t ei_leaf_lo;  // Lower 32-bits of the block number of the extent node that is the next level lower in the tree. The tree node pointed to can be either another internal node or a leaf node, described below.
    u_int16_t ei_leaf_hi;  // Upper 16-bits of the previous field.
    // 0xA __u16 ei_unused;  //
};
int ext4_extent_idx_new(struct ext4_extent_idx * ext4_extend_idx, char * bytes);

struct ext4_extent {
    u_int32_t ee_block;  // First file block number that this extent covers.
    u_int16_t ee_len;  // umber of blocks covered by extent. If the value of this field is <= 32768, the extent is initialized. If the value of the field is > 32768, the extent is uninitialized and the actual extent length is ee_len - 32768. Therefore, the maximum length of a initialized extent is 32768 blocks, and the maximum length of an uninitialized extent is 32767.
    u_int16_t ee_start_hi;  // Upper 16-bits of the block number to which this extent points.
    u_int32_t ee_start_lo;  // Lower 32-bits of the block number to which this extent points.
    u_int64_t ee_start_u64;
};
int ext4_extent_new(struct ext4_extent * ext4_extent, char * bytes);

struct ext4_dir_entry {
    u_int32_t inode;
    u_int16_t rec_len;
    u_int16_t name_len;
    u_char * name;
};
int ext4_dir_entry_new(struct ext4_dir_entry * ext4_dir_entry, char * bytes);

struct ext4_dir_entry_2 {
    u_int32_t inode;
    u_int16_t rec_len;
    u_int8_t name_len;
    u_int8_t file_type;
    u_char * name;
};
int ext4_dir_entry_2_new(struct ext4_dir_entry_2 * ext4_dir_entry_2, char * bytes);

#endif //EXT4_BINARY_READ_INODE_TABLE_H
