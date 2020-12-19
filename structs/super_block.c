//
// Created by wdymel on 2020-12-19.
//

#include "super_block.h"
#include "../interfaces.h"
int SuperBblock_new(struct SuperBlock *superBlock, char *sb_bytes) {
    /* Initializes an instance of superBlock from data in sb_bytes
     * superBlock => pointer to a superBlock
     * sb_bytes => pointer to 1 KiB char array containing the superBlock from the filesystem
     * */

    // region standard fields
    // Total inode count.
    superBlock->s_inodes_count = convert_le_byte_array_to_uint(sb_bytes + 0x0,
                                                               sizeof(superBlock->s_inodes_count));
    // Total block count.
    superBlock->s_blocks_count_lo = convert_le_byte_array_to_uint(sb_bytes + 0x4,
                                                                  sizeof(superBlock->s_blocks_count_lo));
    // This number of blocks can only be allocated by the super-user.
    superBlock->s_r_blocks_count_lo = convert_le_byte_array_to_uint(sb_bytes + 0x8,
                                                                    sizeof(superBlock->s_r_blocks_count_lo));
    // Free block count.
    superBlock->s_free_blocks_count_lo = convert_le_byte_array_to_uint(sb_bytes + 0xC,
                                                                       sizeof(superBlock->s_free_blocks_count_lo));
    // Free inode count.
    superBlock->s_free_inodes_count = convert_le_byte_array_to_uint(sb_bytes + 0x10,
                                                                    sizeof(superBlock->s_free_inodes_count));
    // First data block. This must be at least 1 for 1k-block filesystems and is typically 0 for all other block sizes.
    superBlock->s_first_data_block = convert_le_byte_array_to_uint(sb_bytes + 0x14,
                                                                   sizeof(superBlock->s_first_data_block));
    // Block size is 2 ^ (10 + s_log_block_size).
    superBlock->s_log_block_size = convert_le_byte_array_to_uint(sb_bytes + 0x18,
                                                                 sizeof(superBlock->s_log_block_size));
    // Cluster size is (2 ^ s_log_cluster_size) blocks if bigalloc is enabled. Otherwise s_log_cluster_size must equal s_log_block_size.
    superBlock->s_log_cluster_size = convert_le_byte_array_to_uint(sb_bytes + 0x1C,
                                                                   sizeof(superBlock->s_log_cluster_size));
    // Blocks per group.
    superBlock->s_blocks_per_group = convert_le_byte_array_to_uint(sb_bytes + 0x20,
                                                                   sizeof(superBlock->s_blocks_per_group));
    // Clusters per group, if bigalloc is enabled. Otherwise s_clusters_per_group must equal s_blocks_per_group.
    superBlock->s_clusters_per_group = convert_le_byte_array_to_uint(sb_bytes + 0x24,
                                                                     sizeof(superBlock->s_clusters_per_group));
    // Inodes per group.
    superBlock->s_inodes_per_group = convert_le_byte_array_to_uint(sb_bytes + 0x28,
                                                                   sizeof(superBlock->s_inodes_per_group));
    // Mount time, in seconds since the epoch.
    superBlock->s_mtime = convert_le_byte_array_to_uint(sb_bytes + 0x2C, sizeof(superBlock->s_mtime));
    // Write time, in seconds since the epoch.
    superBlock->s_wtime = convert_le_byte_array_to_uint(sb_bytes + 0x30, sizeof(superBlock->s_wtime));
    // Number of mounts since the last fsck.
    superBlock->s_mnt_count = convert_le_byte_array_to_uint(sb_bytes + 0x34, sizeof(superBlock->s_mnt_count));
    // Number of mounts beyond which a fsck is needed.
    superBlock->s_max_mnt_count = convert_le_byte_array_to_uint(sb_bytes + 0x36,
                                                                sizeof(superBlock->s_max_mnt_count));
    // Magic signature, 0xEF53
    superBlock->s_magic = convert_le_byte_array_to_uint(sb_bytes + 0x38, sizeof(superBlock->s_magic));
    // File system state. Valid values are:
    //0x0001 	Cleanly umounted
    //0x0002 	Errors detected
    //0x0004 	Orphans being recovered
    superBlock->s_state = convert_le_byte_array_to_uint(sb_bytes + 0x3A, sizeof(superBlock->s_state));
    // Behaviour when detecting errors. One of:
    //1 	Continue
    //2 	Remount read-only
    //3 	Panic
    superBlock->s_errors = convert_le_byte_array_to_uint(sb_bytes + 0x3C, sizeof(superBlock->s_errors));
    // endregion

    // Minor revision level.
    superBlock->s_minor_rev_level = convert_le_byte_array_to_uint(sb_bytes + 0x3E,
                                                                  sizeof(superBlock->s_minor_rev_level));
    // Time of last check, in seconds since the epoch.
    superBlock->s_lastcheck = convert_le_byte_array_to_uint(sb_bytes + 0x40, sizeof(superBlock->s_lastcheck));
    // Maximum time between checks, in seconds.
    superBlock->s_checkinterval = convert_le_byte_array_to_uint(sb_bytes + 0x44,
                                                                sizeof(superBlock->s_checkinterval));
    // OS. One of:
    //0 	Linux
    //1 	Hurd
    //2 	Masix
    //3 	FreeBSD
    //4 	Lites
    superBlock->s_creator_os = convert_le_byte_array_to_uint(sb_bytes + 0x48,
                                                             sizeof(superBlock->s_creator_os));
    // Revision level. One of:
    //0 	Original format
    //1 	v2 format w/ dynamic inode sizes
    superBlock->s_rev_level = convert_le_byte_array_to_uint(sb_bytes + 0x4C, sizeof(superBlock->s_rev_level));
    // Default uid for reserved blocks.
    superBlock->s_def_resuid = convert_le_byte_array_to_uint(sb_bytes + 0x50,
                                                             sizeof(superBlock->s_def_resuid));
    // Default gid for reserved blocks.
    superBlock->s_def_resgid = convert_le_byte_array_to_uint(sb_bytes + 0x52,
                                                             sizeof(superBlock->s_def_resgid));
    // endregion

    // region major rev >= 1
    //These fields are for EXT4_DYNAMIC_REV superblocks only.
    //Note: the difference between the compatible feature set and the incompatible feature set is that if there is a bit set in the incompatible feature set that the kernel doesn't know about, it should refuse to mount the filesystem.
    //e2fsck's requirements are more strict; if it doesn't know about a feature in either the compatible or incompatible feature set, it must abort and not try to meddle with things it doesn't understand...
    // First non-reserved inode.
    superBlock->s_first_ino = convert_le_byte_array_to_uint(sb_bytes + 0x54, sizeof(superBlock->s_first_ino));
    // Size of inode structure, in bytes.
    superBlock->s_inode_size = convert_le_byte_array_to_uint(sb_bytes + 0x58,
                                                             sizeof(superBlock->s_inode_size));
    // Block group # of this superblock.
    superBlock->s_block_group_nr = convert_le_byte_array_to_uint(sb_bytes + 0x5A,
                                                                 sizeof(superBlock->s_block_group_nr));
    // Compatible feature set flags. Kernel can still read/write this fs even if it doesn't understand a flag; e2fsck will not attempt to fix a filesystem with any unknown COMPAT flags. Any of:
    //0x1 	Directory preallocation (COMPAT_DIR_PREALLOC).
    //0x2 	"imagic inodes". Used by AFS to indicate inodes that are not linked into the directory namespace. Inodes marked with this flag will not be added to lost+found by e2fsck. (COMPAT_IMAGIC_INODES).
    //0x4 	Has a journal (COMPAT_HAS_JOURNAL).
    //0x8 	Supports extended attributes (COMPAT_EXT_ATTR).
    //0x10 	Has reserved GDT blocks for filesystem expansion. Requires RO_COMPAT_SPARSE_SUPER. (COMPAT_RESIZE_INODE).
    //0x20 	Has indexed directories. (COMPAT_DIR_INDEX).
    //0x40 	"Lazy BG". Not in Linux kernel, seems to have been for uninitialized block groups? (COMPAT_LAZY_BG).
    //0x80 	"Exclude inode". Intended for filesystem snapshot feature, but not used. (COMPAT_EXCLUDE_INODE).
    //0x100 	"Exclude bitmap". Seems to be used to indicate the presence of snapshot-related exclude bitmaps? Not defined in kernel or used in e2fsprogs. (COMPAT_EXCLUDE_BITMAP).
    //0x200 	Sparse Super Block, v2. If this flag is set, the SB field s_backup_bgs points to the two block groups that contain backup superblocks. (COMPAT_SPARSE_SUPER2).
    superBlock->s_feature_compat = convert_le_byte_array_to_uint(sb_bytes + 0x5C,
                                                                 sizeof(superBlock->s_feature_compat));
    // Incompatible feature set. If the kernel or e2fsck doesn't understand one of these bits, it will refuse to mount or attempt to repair the filesystem. Any of:
    //0x1 	Compression. Not implemented. (INCOMPAT_COMPRESSION).
    //0x2 	Directory entries record the file type. See ext4_dir_entry_2 below. (INCOMPAT_FILETYPE).
    //0x4 	Filesystem needs journal recovery. (INCOMPAT_RECOVER).
    //0x8 	Filesystem has a separate journal device. (INCOMPAT_JOURNAL_DEV).
    //0x10 	Meta block groups. See the earlier discussion of this feature. (INCOMPAT_META_BG).
    //0x40 	Files in this filesystem use extents. (INCOMPAT_EXTENTS).
    //0x80 	Enable a filesystem size over 2^32 blocks. (INCOMPAT_64BIT).
    //0x100 	Multiple mount protection. Prevent multiple hosts from mounting the filesystem concurrently by updating a reserved block periodically while mounted and checking this at mount time to determine if the filesystem is in use on another host. (INCOMPAT_MMP).
    //0x200 	Flexible block groups. See the earlier discussion of this feature. (INCOMPAT_FLEX_BG).
    //0x400 	Inodes can be used to store large extended attribute values (INCOMPAT_EA_INODE).
    //0x1000 	Data in directory entry. Allow additional data fields to be stored in each dirent, after struct ext4_dirent. The presence of extra data is indicated by flags in the high bits of ext4_dirent file type flags (above EXT4_FT_MAX). The flag EXT4_DIRENT_LUFID = 0x10 is used to store a 128-bit File Identifier for Lustre. The flag EXT4_DIRENT_IO64 = 0x20 is used to store the high word of 64-bit inode numbers. Feature still in development. (INCOMPAT_DIRDATA).
    //0x2000 	Metadata checksum seed is stored in the superblock. This feature enables the administrator to change the UUID of a metadata_csum filesystem while the filesystem is mounted; without it, the checksum definition requires all metadata blocks to be rewritten. (INCOMPAT_CSUM_SEED).
    //0x4000 	Large directory >2GB or 3-level htree. Prior to this feature, directories could not be larger than 4GiB and could not have an htree more than 2 levels deep. If this feature is enabled, directories can be larger than 4GiB and have a maximum htree depth of 3. (INCOMPAT_LARGEDIR).
    //0x8000 	Data in inode. Small files or directories are stored directly in the inode i_blocks and/or xattr space. (INCOMPAT_INLINE_DATA).
    //0x10000 	Encrypted inodes are present on the filesystem (INCOMPAT_ENCRYPT).
    superBlock->s_feature_incompat = convert_le_byte_array_to_uint(sb_bytes + 0x60,
                                                                   sizeof(superBlock->s_feature_incompat));
    // Readonly-compatible feature set. If the kernel doesn't understand one of these bits, it can still mount read-only, but e2fsck will refuse to modify the filesystem. Any of:
    //0x1 	Sparse superblocks. See the earlier discussion of this feature. (RO_COMPAT_SPARSE_SUPER).
    //0x2 	Allow storing files larger than 2GiB (RO_COMPAT_LARGE_FILE).
    //0x4 	Was intended for use with htree directories, but was not needed. Not used in kernel or e2fsprogs (RO_COMPAT_BTREE_DIR).
    //0x8 	This filesystem has files whose space usage is stored in i_blocks in units of filesystem blocks, not 512-byte sectors. Inodes using this feature will be marked with EXT4_INODE_HUGE_FILE. (RO_COMPAT_HUGE_FILE)
    //0x10 	Group descriptors have checksums. In addition to detecting corruption, this is useful for lazy formatting with uninitialized groups (RO_COMPAT_GDT_CSUM).
    //0x20 	Indicates that the old ext3 32,000 subdirectory limit no longer applies. A directory's i_links_count will be set to 1 if it is incremented past 64,999. (RO_COMPAT_DIR_NLINK).
    //0x40 	Indicates that large inodes exist on this filesystem, storing extra fields after EXT2_GOOD_OLD_INODE_SIZE. (RO_COMPAT_EXTRA_ISIZE).
    //0x80 	This filesystem has a snapshot. Not implemented in ext4. (RO_COMPAT_HAS_SNAPSHOT).
    //0x100 	Quota is handled transactionally with the journal (RO_COMPAT_QUOTA).
    //0x200 	This filesystem supports "bigalloc", which means that filesystem block allocation bitmaps are tracked in units of clusters (of blocks) instead of blocks (RO_COMPAT_BIGALLOC).
    //0x400 	This filesystem supports metadata checksumming. (RO_COMPAT_METADATA_CSUM; implies RO_COMPAT_GDT_CSUM, though GDT_CSUM must not be set)
    //0x800 	Filesystem supports replicas. This feature is neither in the kernel nor e2fsprogs. (RO_COMPAT_REPLICA).
    //0x1000 	Read-only filesystem image; the kernel will not mount this image read-write and most tools will refuse to write to the image. (RO_COMPAT_READONLY).
    //0x2000 	Filesystem tracks project quotas. (RO_COMPAT_PROJECT)
    superBlock->s_feature_ro_compat = convert_le_byte_array_to_uint(sb_bytes + 0x64,
                                                                    sizeof(superBlock->s_feature_ro_compat));
    // For compression (Not used in e2fsprogs/Linux)
    superBlock->s_algorithm_usage_bitmap = convert_le_byte_array_to_uint(sb_bytes + 0xC8,
                                                                         sizeof(superBlock->s_algorithm_usage_bitmap));
    //Performance hints. Directory preallocation should only happen if the EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on.
    // Number of reserved GDT entries for future filesystem expansion.
    superBlock->s_reserved_gdt_blocks = convert_le_byte_array_to_uint(sb_bytes + 0xCE,
                                                                      sizeof(superBlock->s_reserved_gdt_blocks));
    //Journaling support valid if EXT4_FEATURE_COMPAT_HAS_JOURNAL set.
    // inode number of journal file.
    superBlock->s_journal_inum = convert_le_byte_array_to_uint(sb_bytes + 0xE0,
                                                               sizeof(superBlock->s_journal_inum));
    // Device number of journal file, if the external journal feature flag is set.
    superBlock->s_journal_dev = convert_le_byte_array_to_uint(sb_bytes + 0xE4,
                                                              sizeof(superBlock->s_journal_dev));
    // Start of list of orphaned inodes to delete.
    superBlock->s_last_orphan = convert_le_byte_array_to_uint(sb_bytes + 0xE8,
                                                              sizeof(superBlock->s_last_orphan));
    // HTREE hash seed.
    //0x0 	Legacy.
    //0x1 	Half MD4.
    //0x2 	Tea.
    //0x3 	Legacy, unsigned.
    //0x4 	Half MD4, unsigned.
    //0x5 	Tea, unsigned.
    superBlock->s_hash_seed = convert_le_byte_array_to_uint(sb_bytes + 0xEC, sizeof(superBlock->s_hash_seed));
    // Size of group descriptors, in bytes, if the 64bit incompat feature flag is set.
    superBlock->s_desc_size = convert_le_byte_array_to_uint(sb_bytes + 0xFE, sizeof(superBlock->s_desc_size));
    // Default mount options. Any of:
    //0x0001 	Print debugging info upon (re)mount. (EXT4_DEFM_DEBUG)
    //0x0002 	New files take the gid of the containing directory (instead of the fsgid of the current process). (EXT4_DEFM_BSDGROUPS)
    //0x0004 	Support userspace-provided extended attributes. (EXT4_DEFM_XATTR_USER)
    //0x0008 	Support POSIX access control lists (ACLs). (EXT4_DEFM_ACL)
    //0x0010 	Do not support 32-bit UIDs. (EXT4_DEFM_UID16)
    //0x0020 	All data and metadata are commited to the journal. (EXT4_DEFM_JMODE_DATA)
    //0x0040 	All data are flushed to the disk before metadata are committed to the journal. (EXT4_DEFM_JMODE_ORDERED)
    //0x0060 	Data ordering is not preserved; data may be written after the metadata has been written. (EXT4_DEFM_JMODE_WBACK)
    //0x0100 	Disable write flushes. (EXT4_DEFM_NOBARRIER)
    //0x0200 	Track which blocks in a filesystem are metadata and therefore should not be used as data blocks. This option will be enabled by default on 3.18, hopefully. (EXT4_DEFM_BLOCK_VALIDITY)
    //0x0400 	Enable DISCARD support, where the storage device is told about blocks becoming unused. (EXT4_DEFM_DISCARD)
    //0x0800 	Disable delayed allocation. (EXT4_DEFM_NODELALLOC)
    superBlock->s_default_mount_opts = convert_le_byte_array_to_uint(sb_bytes + 0x100,
                                                                     sizeof(superBlock->s_default_mount_opts));
    // endregion

    // region 64 BIT ONLY  TODO only read it if 64 bit flag is set
    // First metablock block group, if the meta_bg feature is enabled.
    superBlock->s_first_meta_bg = convert_le_byte_array_to_uint(sb_bytes + 0x104,
                                                                sizeof(superBlock->s_first_meta_bg));
    // When the filesystem was created, in seconds since the epoch.
    superBlock->s_mkfs_time = convert_le_byte_array_to_uint(sb_bytes + 0x108,
                                                            sizeof(superBlock->s_mkfs_time));
    // Backup copy of the journal inode's i_block[] array in the first 15 elements and i_size_high and i_size in the 16th and 17th elements, respectively.
    superBlock->s_jnl_blocks = convert_le_byte_array_to_uint(sb_bytes + 0x10C,
                                                             sizeof(superBlock->s_jnl_blocks));

    //64bit support valid if EXT4_FEATURE_COMPAT_64BIT
    // High 32-bits of the block count.
    superBlock->s_blocks_count_hi = convert_le_byte_array_to_uint(sb_bytes + 0x150,
                                                                  sizeof(superBlock->s_blocks_count_hi));
    // High 32-bits of the reserved block count.
    superBlock->s_r_blocks_count_hi = convert_le_byte_array_to_uint(sb_bytes + 0x154,
                                                                    sizeof(superBlock->s_r_blocks_count_hi));
    // High 32-bits of the free block count.
    superBlock->s_free_blocks_count_hi = convert_le_byte_array_to_uint(sb_bytes + 0x158,
                                                                       sizeof(superBlock->s_free_blocks_count_hi));
    // All inodes have at least # bytes.
    superBlock->s_min_extra_isize = convert_le_byte_array_to_uint(sb_bytes + 0x15C,
                                                                  sizeof(superBlock->s_min_extra_isize));
    // New inodes should reserve # bytes.
    superBlock->s_want_extra_isize = convert_le_byte_array_to_uint(sb_bytes + 0x15E,
                                                                   sizeof(superBlock->s_want_extra_isize));
    // Miscellaneous flags. Any of:
    //0x0001 	Signed directory hash in use.
    //0x0002 	Unsigned directory hash in use.
    //0x0004 	To test development code.
    superBlock->s_flags = convert_le_byte_array_to_uint(sb_bytes + 0x160, sizeof(superBlock->s_flags));
    // RAID stride. This is the number of logical blocks read from or written to the disk before moving to the next disk. This affects the placement of filesystem metadata, which will hopefully make RAID storage faster.
    superBlock->s_raid_stride = convert_le_byte_array_to_uint(sb_bytes + 0x164,
                                                              sizeof(superBlock->s_raid_stride));
    // # seconds to wait in multi-mount prevention (MMP) checking. In theory, MMP is a mechanism to record in the superblock which host and device have mounted the filesystem, in order to prevent multiple mounts. This feature does not seem to be implemented...
    superBlock->s_mmp_interval = convert_le_byte_array_to_uint(sb_bytes + 0x166,
                                                               sizeof(superBlock->s_mmp_interval));
    // Block # for multi-mount protection data.
    superBlock->s_mmp_block = convert_le_byte_array_to_uint(sb_bytes + 0x168,
                                                            sizeof(superBlock->s_mmp_block));
    // RAID stripe width. This is the number of logical blocks read from or written to the disk before coming back to the current disk. This is used by the block allocator to try to reduce the number of read-modify-write operations in a RAID5/6.
    //0x176 	__le16 	s_reserved_pad
    superBlock->s_raid_stripe_width = convert_le_byte_array_to_uint(sb_bytes + 0x170,
                                                                    sizeof(superBlock->s_raid_stripe_width));
    superBlock->s_log_groups_per_flex = convert_le_byte_array_to_uint(sb_bytes + 0x174,
                                                                      sizeof(superBlock->s_log_groups_per_flex));
    // Number of KiB written to this filesystem over its lifetime.
    superBlock->s_kbytes_written = convert_le_byte_array_to_uint(sb_bytes + 0x178,
                                                                 sizeof(superBlock->s_kbytes_written));
    // inode number of active snapshot. (Not used in e2fsprogs/Linux.)
    superBlock->s_snapshot_inum = convert_le_byte_array_to_uint(sb_bytes + 0x180,
                                                                sizeof(superBlock->s_snapshot_inum));
    // Sequential ID of active snapshot. (Not used in e2fsprogs/Linux.)
    superBlock->s_snapshot_id = convert_le_byte_array_to_uint(sb_bytes + 0x184,
                                                              sizeof(superBlock->s_snapshot_id));
    // Number of blocks reserved for active snapshot's future use. (Not used in e2fsprogs/Linux.)
    superBlock->s_snapshot_r_blocks_count = convert_le_byte_array_to_uint(sb_bytes + 0x188,
                                                                          sizeof(superBlock->s_snapshot_r_blocks_count));
    // inode number of the head of the on-disk snapshot list. (Not used in e2fsprogs/Linux.)
    superBlock->s_snapshot_list = convert_le_byte_array_to_uint(sb_bytes + 0x190,
                                                                sizeof(superBlock->s_snapshot_list));
    // Number of errors seen.
    superBlock->s_error_count = convert_le_byte_array_to_uint(sb_bytes + 0x194,
                                                              sizeof(superBlock->s_error_count));
    // First time an error happened, in seconds since the epoch.
    superBlock->s_first_error_time = convert_le_byte_array_to_uint(sb_bytes + 0x198,
                                                                   sizeof(superBlock->s_first_error_time));
    // inode involved in first error.
    superBlock->s_first_error_ino = convert_le_byte_array_to_uint(sb_bytes + 0x19C,
                                                                  sizeof(superBlock->s_first_error_ino));
    // Number of block involved of first error.
    superBlock->s_first_error_block = convert_le_byte_array_to_uint(sb_bytes + 0x1A0,
                                                                    sizeof(superBlock->s_first_error_block));
    // Line number where error happened.
    superBlock->s_first_error_line = convert_le_byte_array_to_uint(sb_bytes + 0x1C8,
                                                                   sizeof(superBlock->s_first_error_line));
    // Time of most recent error, in seconds since the epoch.
    superBlock->s_last_error_time = convert_le_byte_array_to_uint(sb_bytes + 0x1CC,
                                                                  sizeof(superBlock->s_last_error_time));
    // inode involved in most recent error.
    superBlock->s_last_error_ino = convert_le_byte_array_to_uint(sb_bytes + 0x1D0,
                                                                 sizeof(superBlock->s_last_error_ino));
    // Line number where most recent error happened.
    superBlock->s_last_error_line = convert_le_byte_array_to_uint(sb_bytes + 0x1D4,
                                                                  sizeof(superBlock->s_last_error_line));
    // Number of block involved in most recent error.
    superBlock->s_last_error_block = convert_le_byte_array_to_uint(sb_bytes + 0x1D8,
                                                                   sizeof(superBlock->s_last_error_block));
    // Inode number of user quota file.
    superBlock->s_usr_quota_inum = convert_le_byte_array_to_uint(sb_bytes + 0x240,
                                                                 sizeof(superBlock->s_usr_quota_inum));
    // Inode number of group quota file.
    superBlock->s_grp_quota_inum = convert_le_byte_array_to_uint(sb_bytes + 0x244,
                                                                 sizeof(superBlock->s_grp_quota_inum));
    // Overhead blocks/clusters in fs. (Huh? This field is always zero, which means that the kernel calculates it dynamically.)
    superBlock->s_overhead_blocks = convert_le_byte_array_to_uint(sb_bytes + 0x248,
                                                                  sizeof(superBlock->s_overhead_blocks));
    // Block groups containing superblock backups (if sparse_super2)
    //0 	Invalid algorithm (ENCRYPTION_MODE_INVALID).
    //1 	256-bit AES in XTS mode (ENCRYPTION_MODE_AES_256_XTS).
    //2 	256-bit AES in GCM mode (ENCRYPTION_MODE_AES_256_GCM).
    //3 	256-bit AES in CBC mode (ENCRYPTION_MODE_AES_256_CBC).
    superBlock->s_backup_bgs = convert_le_byte_array_to_uint(sb_bytes + 0x24C,
                                                             sizeof(superBlock->s_backup_bgs));
    // Inode number of lost+found
    superBlock->s_lpf_ino = convert_le_byte_array_to_uint(sb_bytes + 0x268, sizeof(superBlock->s_lpf_ino));
    // Inode that tracks project quotas.
    superBlock->s_prj_quota_inum = convert_le_byte_array_to_uint(sb_bytes + 0x26C,
                                                                 sizeof(superBlock->s_prj_quota_inum));
    // Checksum seed used for metadata_csum calculations. This value is crc32c(~0, $orig_fs_uuid).
    superBlock->s_checksum_seed = convert_le_byte_array_to_uint(sb_bytes + 0x270,
                                                                sizeof(superBlock->s_checksum_seed));
    // Padding to the end of the block.
    superBlock->s_reserved = convert_le_byte_array_to_uint(sb_bytes + 0x274, sizeof(superBlock->s_reserved));
    // Superblock checksum.
    superBlock->s_checksum = convert_le_byte_array_to_uint(sb_bytes + 0x3FC, sizeof(superBlock->s_checksum));
    // endregion

    // region calc
    superBlock->s_blocks_count_u64 = ((u_int64_t) superBlock->s_blocks_count_hi << 32u) + superBlock->s_blocks_count_lo;
    superBlock->s_r_blocks_count_u64 = ((u_int64_t) superBlock->s_r_blocks_count_hi << 32u) + superBlock->s_r_blocks_count_lo;
    superBlock->s_free_blocks_count_u64 = ((u_int64_t) superBlock->s_free_blocks_count_hi << 32u) + superBlock->s_free_blocks_count_lo;
    superBlock->s_block_size = 1024u << superBlock->s_log_block_size;
    superBlock->s_cluster_size = (u_int64_t)1u << superBlock->s_log_cluster_size;
    superBlock->s_first_group_desc_block = superBlock->s_block_size == 1024 ? 2 : 1;
    superBlock->s_groups_per_flex = (u_int64_t)1u << superBlock->s_log_groups_per_flex;
    // endregion

    return 0;
}