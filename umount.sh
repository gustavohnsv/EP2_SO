#!/bin/bash

sudo pkill -f '/mnt/ramdisk'

umount /mnt/ramdisk 2>/dev/null

if mount | grep '/mnt/ramdisk' > /dev/null; then
    sudo fuser -km /mnt/ramdisk
    sudo umount /mnt/ramdisk
fi

if ! mount | grep '/mnt/ramdisk' > /dev/null; then
    rm -rf /mnt/ramdisk
fi