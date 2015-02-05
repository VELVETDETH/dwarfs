
#ifndef DWARFS_H__
#define DWARFS_H__

#define DWARFS_MAGIC 0x19940303
#define DWARFS_DEFAULT_BLOCK_SIZE 4 * 1024

struct dwarfs_super_block
{
    unsigned int version;
    unsigned int magic;
    unsigned int block_size;
    unsigned int free_blocks;

    char padding[DWARFS_DEFAULT_BLOCK_SIZE - 4*sizeof(unsigned int)];
};

#endif
