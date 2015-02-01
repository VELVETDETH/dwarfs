
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

struct inode *dwarfs_get_inode(struct super_block *sb,
                               const struct inode *dir,
                               umode_t mode,
                               dev_t dev)
{
    struct inode *inode = new_inode(sb);
    if (inode)
    {
        inode->i_ino = get_next_ino();
        inode_init_owner(inode, dir, mode);

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

    sb->s_magic = 0x00940303;

    inode = dwarfs_get_inode(sb, NULL, S_IFDIR, 0);
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
    printk(KERN_INFO,
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
