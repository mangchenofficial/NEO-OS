@echo off
set PATH=C:\Program Files\NASM;C:\msys64\mingw64\bin;C:\Program Files\qemu;%PATH%

echo === NeoOS Build ===

echo Cleaning up...
del /Q *.o 2>nul
del /Q *.bin 2>nul
del /Q *.elf 2>nul
del /Q *.exe 2>nul
del /Q os.img 2>nul

echo Step 1: Assembling boot sector...
nasm -f bin boot.asm -o boot.bin

echo Step 2: Assembling ISR...
nasm -f win32 isr.asm -o isr.o
nasm -f win32 entry.asm -o entry.o

echo Step 3: Compiling kernel...
gcc -m32 -ffreestanding -c kernel.c -o kernel.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c screen.c -o screen.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c keyboard.c -o keyboard.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c shell.c -o shell.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c memory.c -o memory.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c heap.c -o heap.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c idt.c -o idt.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c pic.c -o pic.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c irq.c -o irq.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c block.c -o block.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c fat12.c -o fat12.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c vfs.c -o vfs.o -std=c11 -fno-pie -fno-leading-underscore
gcc -m32 -ffreestanding -c string.c -o string.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c elf.c -o elf.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c task.c -o task.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c syscall.c -o syscall.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c video.c -o video.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c font.c -o font.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c mouse.c -o mouse.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c window.c -o window.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c desktop.c -o desktop.o -std=c11 -fno-pie -fno-leading-underscore
    gcc -m32 -ffreestanding -c gshell.c -o gshell.o -std=c11 -fno-pie -fno-leading-underscore

echo Step 4: Linking kernel (PE format)...
gcc -m32 -ffreestanding -nostdlib -Wl,-Tlinker.ld entry.o kernel.o screen.o keyboard.o shell.o memory.o heap.o idt.o pic.o irq.o isr.o block.o fat12.o vfs.o string.o elf.o task.o syscall.o video.o font.o mouse.o window.o desktop.o gshell.o

echo Step 5: Extracting binary...
objcopy -O binary kernel.exe kernel.bin

echo Step 6: Creating disk image...
powershell -ExecutionPolicy Bypass -File create_fat12.ps1

echo === Build complete! ===
echo Run: qemu-system-i386 -drive format=raw,file=os.img
