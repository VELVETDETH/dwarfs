
#include "dwarfs.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

    // allocate super block is just writing on a device
    ret = write(fd, (char *)&sb, sizeof(sb));

    if (ret != DWARFS_DEFAULT_BLOCK_SIZE)
        printf("Bytes [%d] are not equal to default block size\n", (int)ret);
    else
        printf("Super block written succesively\n");

    close(fd);

    return 0;
}
