$env:PATH = "C:\Program Files\NASM;C:\msys64\mingw64\bin;C:\Program Files\qemu;$env:PATH"

Write-Host "Step 1: Assembling boot sector..."
nasm -f bin boot.asm -o boot.bin
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Step 1.5: Assembling isr.asm..."
nasm -f elf32 isr.asm -o isr.o
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Step 2: Compiling kernel..."
gcc -m32 -ffreestanding -fno-leading-underscore -c kernel.c -o kernel.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c screen.c -o screen.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c keyboard.c -o keyboard.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c shell.c -o shell.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c memory.c -o memory.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c heap.c -o heap.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c idt.c -o idt.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c pic.c -o pic.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -fno-leading-underscore -c irq.c -o irq.o -std=c11 -fno-pie
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Step 3: Linking to ELF..."
$ldArgs = @("-m32", "-ffreestanding", "-nostdlib", "-Wl,-Ttext=0x100000", "-o", "kernel.elf", "kernel.o", "screen.o", "keyboard.o", "shell.o", "memory.o", "heap.o", "idt.o", "pic.o", "irq.o", "isr.o")
gcc @ldArgs
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Step 4: Extracting binary..."
objcopy -O binary kernel.elf kernel.bin
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Step 5: Creating image..."
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

Write-Host "Build successful!"
