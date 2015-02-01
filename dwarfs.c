
#include <linux/init.h>
#include <linux/module.h>

static int dwarfs_init(void)
{
    printk(KERN_ALERT "Dwarfs Init\n");
    return 0;
}

static void dwarfs_exit(void)
{
    printk(KERN_ALERT "Dwarfs Exited\n");
}

module_init(dwarfs_init);
module_exit(dwarfs_exit);
