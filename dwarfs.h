
#ifndef DWARFS_H__
#define DWARFS_H__

// what specifies a file system?
// 1. magic number
// 2. default block size
//
#define DWARFS_MAGIC 0x19940303
#define DWARFS_DEFAULT_BLOCK_SIZE 4096
#define DWARFS_FILENAME_MAXLEN 255

const int DWARFS_ROOT_INODE_NUMBER = 1;
const int DWARFS_SUPERBLOCK_BLOCK_NUMBER = 0;
const int DWARFS_INODESTORE_BLOCK_NUMBER = 1;
const int DWARFS_ROOTDIR_DATABLOCK_NUMBER = 2;

struct dwarfs_dir_record
{
    char filename[DWARFS_FILENAME_MAXLEN];
    uint32_t inode_no;
};

struct dwarfs_inode
{
    mode_t mode;
    uint32_t inode_no;
    uint32_t data_block_number;

    union // could be file or directory
    {
        uint32_t file_size;
        uint32_t dir_children_count;
    };
};

struct dwarfs_super_block
{
    uint32_t version;
    uint32_t magic;
    uint32_t block_size;
    uint32_t free_blocks;

    struct dwarfs_inode root_inode;

    char padding[DWARFS_DEFAULT_BLOCK_SIZE - 4*sizeof(uint32_t) - sizeof(struct dwarfs_inode)];
};

#endif
