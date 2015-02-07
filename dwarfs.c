
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "super.h"

static int dwarfs_readdir(struct file *fp, struct dir_context* dir)
{
    loff_t pos = fp->f_pos;
    struct inode *inode = fp->f_dentry->d_inode;
    struct super_block *sb = inode->i_sb;
    struct buffer_head *bh;
    struct dwarfs_inode *d_fs_inode;
    struct dwarfs_dir_record *d_fs_dir_record;
    int i;

    printk(KERN_INFO "Reading dir... The pos[%lld], inode number[%lu], superblock magic [%lu]\n",
            pos,
            inode->i_ino,
            sb->s_magic);

    d_fs_inode = inode->i_private;

    if (unlikely(!S_ISDIR(d_fs_inode->mode)))
    {
        printk(KERN_ERR "inode %u not a directory.\n", d_fs_inode->inode_no);
        return -ENOTDIR;
    }
    bh = (struct buffer_head *) sb_bread(sb, d_fs_inode->data_block_number);

    d_fs_dir_record = (struct dwarfs_dir_record*) bh->b_data;
    for (i = 0; i < d_fs_inode->dir_children_count; i++)
    {
        printk(KERN_INFO "Got filename: %s\n", d_fs_dir_record->filename);
        pos += sizeof(struct dwarfs_dir_record);
    }

    return 1;
}

const struct file_operations dwarfs_dir_operations = {
    .owner = THIS_MODULE,
    .iterate = dwarfs_readdir,
};

struct dentry *dwarfs_lookup(struct inode *parent_inode,
                             struct dentry *child_dentry,
                             unsigned int flags)
{
    return NULL;
}

static struct inode_operations dwarfs_inode_operations = {
    .lookup = dwarfs_lookup,
};

struct inode *dwarfs_get_inode(struct super_block *sb,
                               const struct inode *dir,
                               umode_t mode,
                               dev_t dev)
{
    // set up i_state and i_sb_list
    struct inode *inode = new_inode(sb);
    if (inode) // if it's ok
    {
        // index number of inode
        inode->i_ino = get_next_ino();
        // set up several authorities
        inode_init_owner(inode, dir, mode);
        // time variables
        inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;

        switch (mode & S_IFMT)
        {
            case S_IFDIR:
                inc_nlink(inode);
                break;
            case S_IFREG:
            case S_IFLNK:
            default:
                printk(KERN_ERR
                       "dwarfs can create meaningful inode \
                       for only root directory at the moment\n");
                return NULL;
                break;
        }
    }
    return inode;
}

int dwarfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *root_inode;

    // read super_block from disk;
    // by using "block cache"
    struct buffer_head *bh;
    struct dwarfs_super_block *sb_disk;

    // sb here provides bdev and block_size information
    bh = (struct buffer_head *) sb_bread(sb, 0);
    // transform block data by type casting
    sb_disk = (struct dwarfs_super_block *) bh->b_data;

    // now check the device, to see if it has been occupied by other file systems.

    printk(KERN_INFO "The magic number obtained in disk is [%d]\n",
            sb_disk->magic);

    if (unlikely(sb_disk->magic != DWARFS_MAGIC))
    {
        printk(KERN_ERR
                "The filesystem you try to mount is not the type of "
                "dwarfs. Magic number mismatched.\n");
        return -EPERM;
    }

    if (unlikely(sb_disk->block_size != DWARFS_DEFAULT_BLOCK_SIZE))
    {
        printk(KERN_ERR
                "dwarfs seem to be formatted using a non-standard block size.\n");
        return -EPERM;
    }

    printk(KERN_INFO
            "dwarfs filesystem of version [%d] formatted with "
            "a block size of [%d] detected in the device.\n",
            sb_disk->version,
            sb_disk->block_size);

    sb->s_magic = DWARFS_MAGIC;

    sb->s_fs_info = sb_disk;

    root_inode = new_inode(sb);
    root_inode->i_ino = DWARFS_ROOT_INODE_NUMBER;
    inode_init_owner(root_inode, NULL, S_IFDIR);
    root_inode->i_sb = sb;
    root_inode->i_op = &dwarfs_inode_operations;
    root_inode->i_fop = &dwarfs_dir_operations;
    root_inode->i_atime = root_inode->i_ctime = CURRENT_TIME;
    root_inode->i_private = &(sb_disk->root_inode);

    sb->s_root = d_make_root(root_inode);
    if (!sb->s_root)
        return -ENOMEM;
    return 0;
}

static struct dentry *dwarfs_mount(struct file_system_type *fs_type,
                                   int flags,
                                   const char *dev_name,
                                   void *data)
{
    struct dentry *ret;

    // before mount fs
    printk(KERN_INFO "dwarfs is mounting ...\n");

    ret = mount_bdev(fs_type, flags, dev_name, data, dwarfs_fill_super);

    if (unlikely(IS_ERR(ret)))
        printk(KERN_ERR "Error mounting dwarfs");
    else
        printk(KERN_INFO "dwarfs is succesfully mounted on [%s]\n",
               dev_name);
    return ret;
}

static void dwarfs_kill_superblock(struct super_block *s)
{
    printk(KERN_INFO
            "dwarfs superblock is destroyed. Unmount successful.\n");
    return ;
}

// add basic file system functionalities
struct file_system_type dwarfs_fs_type = {
    .owner = THIS_MODULE,
    .name = "dwarfs",
    .mount = dwarfs_mount,
    .kill_sb = dwarfs_kill_superblock,
};

static int dwarfs_init(void)
{
    int ret;

    ret = register_filesystem(&dwarfs_fs_type);
    if (likely(ret==0))
        printk(KERN_INFO "Successfully registered dwarfs\n");
    else
        printk(KERN_ERR "Failed to register dwarfs. Error:[%d]\n", ret);

    return ret;
}

static void dwarfs_exit(void)
{
    int ret;

    ret = unregister_filesystem(&dwarfs_fs_type);
    if (likely(ret==0))
        printk(KERN_INFO "Successfully unregistered dwarfs\n");
    else
        printk(KERN_ERR "Failed to unregister dwarfs. Error:[%d]",
                ret);
}

module_init(dwarfs_init);
module_exit(dwarfs_exit);
