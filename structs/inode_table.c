//
// Created by wdymel on 2020-12-19.
//

#include "inode_table.h"
#include "../interfaces.h"
#include <string.h>
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
    // Inode version. However, if the EA_INODE inode flag is set, this inode stores an extended attribute value and this field contains the upper 32 bits of the attribute value's reference count.
    inodeTable->l_i_version = convert_le_byte_array_to_uint(sb_bytes + 0x24, sizeof(inodeTable->l_i_version));
    // Block map or extent tree. See the section "The Contents of inode.i_block".
    inodeTable->i_block;
    memcpy(inodeTable->i_block, sb_bytes + 0x28, 60);
    // File version (for NFS).
    inodeTable->i_generation = convert_le_byte_array_to_uint(sb_bytes + 0x64, sizeof(inodeTable->i_generation));
    // Lower 32-bits of extended attribute block. ACLs are of course one of many possible extended attributes; I think the name of this field is a result of the first use of extended attributes being for ACLs.
    inodeTable->i_file_acl_lo = convert_le_byte_array_to_uint(sb_bytes + 0x68, sizeof(inodeTable->i_file_acl_lo));
    // Upper 32-bits of file/directory size. In ext2/3 this field was named i_dir_acl, though it was usually set to zero and never used.
    inodeTable->i_size_high= convert_le_byte_array_to_uint(sb_bytes + 0x6C, sizeof(inodeTable->i_size_high));
    // (Obsolete) fragment address.
    inodeTable->i_obso_faddr = convert_le_byte_array_to_uint(sb_bytes + 0x70, sizeof(inodeTable->i_obso_faddr));

    // Upper 16-bits of the block count. Please see the note attached to i_blocks_lo.
    inodeTable->l_i_blocks_high = convert_le_byte_array_to_uint(sb_bytes + 0x74 + 0x0, sizeof(inodeTable->l_i_blocks_high));
    // Upper 16-bits of the extended attribute block (historically, the file ACL location). See the Extended Attributes section below.
    inodeTable->l_i_file_acl_high = convert_le_byte_array_to_uint(sb_bytes + 0x74 + 0x2, sizeof(inodeTable->l_i_file_acl_high));
    // Upper 16-bits of the Owner UID.
    inodeTable->l_i_uid_high = convert_le_byte_array_to_uint(sb_bytes + 0x74 + 0x4, sizeof(inodeTable->l_i_uid_high));
    // Upper 16-bits of the GID.
    inodeTable->l_i_gid_high = convert_le_byte_array_to_uint(sb_bytes + 0x74 + 0x6, sizeof(inodeTable->l_i_gid_high));
    // Lower 16-bits of the inode checksum.
    inodeTable->l_i_checksum_lo = convert_le_byte_array_to_uint(sb_bytes + 0x74 + 0x8, sizeof(inodeTable->l_i_checksum_lo));
    // Unused.
    inodeTable->l_i_reserved = convert_le_byte_array_to_uint(sb_bytes + 0x74 + 0xA, sizeof(inodeTable->l_i_reserved));

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

    inodeTable->i_size_u64 = ((u_int64_t) inodeTable->i_size_high << 32u) + inodeTable->i_size_lo;
    inodeTable->i_blocks_u64 = ((u_int64_t) inodeTable->l_i_blocks_high << 32u) + inodeTable->i_blocks_lo;
    return 0;
}

int ext4_extent_header_new(struct ext4_extent_header *ext4_extent_header, char *bytes) {
    ext4_extent_header->eh_magic = convert_le_byte_array_to_uint(bytes + 0x0, sizeof(ext4_extent_header->eh_magic));
    ext4_extent_header->eh_entries = convert_le_byte_array_to_uint(bytes + 0x2, sizeof(ext4_extent_header->eh_entries));
    ext4_extent_header->eh_max = convert_le_byte_array_to_uint(bytes + 0x4, sizeof(ext4_extent_header->eh_max));
    ext4_extent_header->eh_depth = convert_le_byte_array_to_uint(bytes + 0x6, sizeof(ext4_extent_header->eh_depth));
    ext4_extent_header->eh_generation = convert_le_byte_array_to_uint(bytes + 0x8, sizeof(ext4_extent_header->eh_generation));
    return ext4_extent_header->eh_magic != 0xf30a;
}

int ext4_extent_idx_new(struct ext4_extent_idx *ext4_extend_idx, char *bytes) {
    ext4_extend_idx->ei_block = convert_le_byte_array_to_uint(bytes + 0x0, sizeof(ext4_extend_idx->ei_block));
    ext4_extend_idx->ei_leaf_lo = convert_le_byte_array_to_uint(bytes + 0x4, sizeof(ext4_extend_idx->ei_leaf_lo));
    ext4_extend_idx->ei_leaf_hi = convert_le_byte_array_to_uint(bytes + 0x8, sizeof(ext4_extend_idx->ei_leaf_hi));
    ext4_extend_idx->ei_leaf_u64 = ((u_int64_t) ext4_extend_idx->ei_leaf_hi << 32u) + ext4_extend_idx->ei_leaf_lo;
    return 0;
}

int ext4_extent_new(struct ext4_extent *ext4_extent, char *bytes) {
    ext4_extent->ee_block = convert_le_byte_array_to_uint(bytes + 0x0, sizeof(ext4_extent->ee_block));
    ext4_extent->ee_len = convert_le_byte_array_to_uint(bytes + 0x4, sizeof(ext4_extent->ee_len));
    ext4_extent->ee_start_hi = convert_le_byte_array_to_uint(bytes + 0x6, sizeof(ext4_extent->ee_start_hi));
    ext4_extent->ee_start_lo = convert_le_byte_array_to_uint(bytes + 0x8, sizeof(ext4_extent->ee_start_lo));
    ext4_extent->ee_start_u64 = ((u_int64_t) ext4_extent->ee_start_hi << 32u) + ext4_extent->ee_start_lo;
    return 0;
}

int ext4_dir_entry_new(struct ext4_dir_entry *ext4_dir_entry, char *bytes) {
    ext4_dir_entry->inode = convert_le_byte_array_to_uint(bytes + 0x0, sizeof(ext4_dir_entry->inode));
    ext4_dir_entry->rec_len = convert_le_byte_array_to_uint(bytes + 0x4, sizeof(ext4_dir_entry->rec_len));
    ext4_dir_entry->name_len = convert_le_byte_array_to_uint(bytes + 0x6, sizeof(ext4_dir_entry->name_len));
    ext4_dir_entry->name = malloc(ext4_dir_entry->name_len);
    strncpy(ext4_dir_entry->name, bytes + 0x8, ext4_dir_entry->name_len);
    return 0;
}

int ext4_dir_entry_2_new(struct ext4_dir_entry_2 *ext4_dir_entry_2, char *bytes, u_int8_t load_name) {
    ext4_dir_entry_2->inode = convert_le_byte_array_to_uint(bytes + 0x0, sizeof(ext4_dir_entry_2->inode));
    ext4_dir_entry_2->rec_len = convert_le_byte_array_to_uint(bytes + 0x4, sizeof(ext4_dir_entry_2->rec_len));
    ext4_dir_entry_2->name_len = convert_le_byte_array_to_uint(bytes + 0x6, sizeof(ext4_dir_entry_2->name_len));
    ext4_dir_entry_2->file_type = convert_le_byte_array_to_uint(bytes + 0x7, sizeof(ext4_dir_entry_2->file_type));
    if (load_name)
    {
        ext4_dir_entry_2->name = malloc(ext4_dir_entry_2->name_len);
        strncpy(ext4_dir_entry_2->name, bytes + 0x8, ext4_dir_entry_2->name_len);
    }
    return 0;
}
