
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#include "dwarfs.h"

static int dwarfs_iterate(struct file *fp, struct dir_context* dir)
{
    return 0;
}

const struct file_operations dwarfs_dir_operations = {
    .owner = THIS_MODULE,
    .iterate = dwarfs_iterate,
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
    struct inode *inode;

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
        printk(KERN_ERR,
                "dwarfs seem to be formatted using a non-standard block size.\n");
        return -EPERM;
    }

    printk(KERN_INFO
            "dwarfs filesystem of version [%d] formatted with "
            "a block size of [%d] detected in the device.\n",
            sb_disk->version,
            sb_disk->block_size);

    sb->s_magic = DWARFS_MAGIC;

    inode = dwarfs_get_inode(sb, NULL, S_IFDIR, 0);
    inode->i_op = &dwarfs_inode_operations;
    inode->i_fop = &dwarfs_dir_operations;

    sb->s_root = d_make_root(inode);
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
