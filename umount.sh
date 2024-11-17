#!/bin/bash

ps aux | grep '/mnt/ramdisk' | grep -v grep | awk '{print $2}' | xargs sudo kill -9

umount /mnt/ramdisk 2>/dev/null

if mount | grep '/mnt/ramdisk' > /dev/null; then
    sudo fuser -km /mnt/ramdisk
    sudo umount /mnt/ramdisk
fi

rm -rf /mnt/ramdisk