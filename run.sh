#!/bin/bash

# NeoOS 运行脚本

echo "启动 QEMU..."

qemu-system-i386 -drive format=raw,file=os.img -display curses

# 如果没有curses支持，使用默认显示
# qemu-system-i386 -drive format=raw,file=os.img
