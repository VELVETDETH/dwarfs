
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include "dwarfs.h"

int main(int argc, char *argv[])
{
    int fd;
    ssize_t ret;
    struct dwarfs_super_block sb;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [device]\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
    {
        perror("Error Opening the device\n");
        return -1;
    }

    sb.version     = 1;
    sb.magic       = DWARFS_MAGIC;
    sb.block_size  = DWARFS_DEFAULT_BLOCK_SIZE;
    sb.free_blocks = ~0;

    sb.root_inode.mode = S_IFDIR;
    sb.root_inode.inode_no = DWARFS_ROOT_INODE_NUMBER;
    sb.root_inode.data_block_number = DWARFS_ROOTDIR_DATABLOCK_NUMBER;
    sb.root_inode.dir_children_count = 0;

    // allocate super block is just writing on a device
    ret = write(fd, (char *)&sb, sizeof(sb));

    if (ret != DWARFS_DEFAULT_BLOCK_SIZE)
        printf("Bytes [%d] are not equal to default block size\n", (int)ret);
    else
        printf("Super block written succesively\n");

    close(fd);

    return 0;
}
