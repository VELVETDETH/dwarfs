#include "dwarfs.h"

static inline struct dwarfs_super_block *DWARFS_SUPER_BLOCK(struct super_block *sb)
{
    return sb->s_fs_info;
}
