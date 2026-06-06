$env:PATH = "C:\Program Files\NASM;C:\msys64\mingw64\bin;C:\Program Files\qemu;$env:PATH"

Write-Host "=== NEO OS 编译 ===" -ForegroundColor Cyan

Write-Host "第1步: 汇编引导扇区..." -ForegroundColor Yellow
nasm -f bin boot.asm -o boot.bin
if ($LASTEXITCODE -ne 0) { Write-Host "错误!" -ForegroundColor Red; exit 1 }
Write-Host "完成" -ForegroundColor Green

Write-Host "第2步: 汇编中断服务程序..." -ForegroundColor Yellow
nasm -f win32 isr.asm -o isr.o
if ($LASTEXITCODE -ne 0) { Write-Host "错误!" -ForegroundColor Red; exit 1 }
Write-Host "完成" -ForegroundColor Green

Write-Host "第2b步: 汇编入口代码..." -ForegroundColor Yellow
nasm -f win32 entry.asm -o entry.o
if ($LASTEXITCODE -ne 0) { Write-Host "错误!" -ForegroundColor Red; exit 1 }
Write-Host "完成" -ForegroundColor Green

Write-Host "第3步: 编译内核..." -ForegroundColor Yellow
gcc -m32 -ffreestanding -fno-leading-underscore -c kernel.c -o kernel.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c screen.c -o screen.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c keyboard.c -o keyboard.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c shell.c -o shell.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c memory.c -o memory.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c heap.c -o heap.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c idt.c -o idt.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c pic.c -o pic.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c irq.c -o irq.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c task.c -o task.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c syscall.c -o syscall.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c vfs.c -o vfs.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c video.c -o video.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c mouse.c -o mouse.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c desktop.c -o desktop.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c window.c -o window.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c font.c -o font.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c string.c -o string.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c elf.c -o elf.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c block.c -o block.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c fat12.c -o fat12.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c gshell.c -o gshell.o -std=c11 -fno-pie
if ($LASTEXITCODE -ne 0) { Write-Host "错误!" -ForegroundColor Red; exit 1 }
Write-Host "完成" -ForegroundColor Green

Write-Host "第4步: 链接内核 (ELF)..." -ForegroundColor Yellow
gcc -m32 -ffreestanding -fno-leading-underscore -nostdlib -T linker.ld -o kernel.elf entry.o kernel.o screen.o keyboard.o shell.o memory.o heap.o idt.o pic.o irq.o isr.o block.o fat12.o vfs.o string.o elf.o task.o syscall.o video.o font.o mouse.o window.o desktop.o gshell.o
if ($LASTEXITCODE -ne 0) { Write-Host "链接错误!" -ForegroundColor Red; exit 1 }
Write-Host "完成" -ForegroundColor Green

Write-Host "第4b步: 转换为原始二进制..." -ForegroundColor Yellow
C:\msys64\mingw64\bin\objcopy.exe -O binary kernel.elf kernel.bin
if ($LASTEXITCODE -ne 0) { Write-Host "OBJCOPY错误!" -ForegroundColor Red; exit 1 }
Write-Host "完成" -ForegroundColor Green

Write-Host "第5步: 创建磁盘镜像..." -ForegroundColor Yellow
$size = 1474560
$img = New-Object byte[] $size
[System.IO.File]::WriteAllBytes("os.img", $img)
$boot = [System.IO.File]::ReadAllBytes("boot.bin")
$kern = [System.IO.File]::ReadAllBytes("kernel.bin")
$stream = [System.IO.File]::OpenWrite("os.img")
$stream.Write($boot, 0, $boot.Length)
$stream.Position = 512
$stream.Write($kern, 0, $kern.Length)
$stream.Close()
Write-Host "完成" -ForegroundColor Green

Write-Host "=== 编译完成! ===" -ForegroundColor Cyan
Write-Host "运行: qemu-system-i386 -drive format=raw,file=os.img"
