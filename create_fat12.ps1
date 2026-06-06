$ErrorActionPreference = "Stop"

# 常量定义
$IMAGE_SIZE = 1474560  # 1.44MB
$SECTOR_SIZE = 512
$FAT_START = 1
$ROOT_DIR_START = 19
$DATA_START = 33

Write-Host "=== Creating FAT12 Floppy Image ===" -ForegroundColor Green

# 1. 创建空白镜像
Write-Host "Creating blank disk image..." -ForegroundColor Cyan
$img = New-Object byte[] $IMAGE_SIZE

# 2. 读取原始 boot.bin（保留引导代码）
if (Test-Path "boot.bin") {
    $boot = [System.IO.File]::ReadAllBytes("boot.bin")
    Write-Host "Writing original boot sector..." -ForegroundColor Cyan
    [Array]::Copy($boot, 0, $img, 0, [Math]::Min($boot.Length, 512))
}

# 3. 写入内核（从第二个扇区开始）
Write-Host "Writing kernel..." -ForegroundColor Cyan
if (Test-Path "kernel.bin") {
    $kernel = [System.IO.File]::ReadAllBytes("kernel.bin")
    [Array]::Copy($kernel, 0, $img, $SECTOR_SIZE, $kernel.Length)
}

# 保存镜像
[System.IO.File]::WriteAllBytes("os.img", $img)

Write-Host ""
Write-Host "=== Success! ===" -ForegroundColor Green
Write-Host "os.img created"
