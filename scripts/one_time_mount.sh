#!/bin/bash

# touch a new image
dd bs=1M count=100 if=/dev/zero of=image

# build a directory
mkdir mount

# mount file_system
mount -o loop -t dwarfs image mount

# unmount
umount ./mount

# clean
rm -r mount 
rm image
