#!/bin/bash

# NeoOS 构建脚本

echo "开始编译 NeoOS..."

# 编译汇编引导扇区
echo "编译引导扇区..."
nasm -f bin boot.asm -o boot.bin

if [ $? -ne 0 ]; then
    echo "引导扇区编译失败！"
    exit 1
fi

# 编译C内核
echo "编译内核..."
gcc -m32 -ffreestanding -fno-pie -c kernel.c -o kernel.o

if [ $? -ne 0 ]; then
    echo "内核编译失败！"
    exit 1
fi

# 链接内核
echo "链接内核..."
ld -m elf_i386 -T linker.ld kernel.o -o kernel.bin

if [ $? -ne 0 ]; then
    echo "链接失败！"
    exit 1
fi

# 创建软盘镜像
echo "创建镜像..."
dd if=/dev/zero of=os.img bs=1024 count=1440 2>/dev/null
dd if=boot.bin of=os.img conv=notrunc 2>/dev/null
dd if=kernel.bin of=os.img bs=1 seek=512 conv=notrunc 2>/dev/null

echo "编译完成！"
echo "运行: bash run.sh"
