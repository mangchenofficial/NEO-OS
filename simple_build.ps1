$env:PATH="C:\Program Files\NASM;C:\msys64\mingw64\bin;C:\Program Files\qemu;$env:PATH"

Write-Host "Step 1: Boot"
nasm -f bin boot.asm -o boot.bin

Write-Host "Step 2: Compile"
gcc -m32 -ffreestanding -c kernel.c -o kernel.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -c screen.c -o screen.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -c keyboard.c -o keyboard.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -c shell.c -o shell.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -c memory.c -o memory.o -std=c11 -fno-pie
gcc -m32 -ffreestanding -c heap.c -o heap.o -std=c11 -fno-pie

Write-Host "Step 3: Link (use GCC)"
gcc -m32 -ffreestanding -nostdlib -Wl',--oformat=binary' -Wl',-Ttext=0x100000' -o kernel.bin kernel.o screen.o keyboard.o shell.o memory.o heap.o

Write-Host "Step 4: Create image"
$size=1474560
$img = New-Object byte[] $size
[System.IO.File]::WriteAllBytes("os.img", $img)
$boot = [System.IO.File]::ReadAllBytes("boot.bin")
$kern = [System.IO.File]::ReadAllBytes("kernel.bin")
$stream = [System.IO.File]::OpenWrite("os.img")
$stream.Write($boot,0,$boot.Length)
$stream.Position=512
$stream.Write($kern,0,$kern.Length)
$stream.Close()

Write-Host "Done!"
