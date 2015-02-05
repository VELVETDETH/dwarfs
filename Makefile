obj-m := dwarfs_module.o
dwarfs_module-objs := dwarfs.o

all: ko make-dwarfs

ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

make-dwarfs_SOURCES:
	make-dwarfs.c dwarfs.h

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

