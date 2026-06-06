$qemuPath = "C:\Program Files\qemu\qemu-system-i386.exe"

if (-not (Test-Path $qemuPath)) {
    Write-Host "Error: QEMU not found at $qemuPath" -ForegroundColor Red
    exit 1
}

Write-Host "Starting NeoOS..." -ForegroundColor Green
Start-Process -FilePath $qemuPath -ArgumentList "-drive","format=raw,file=os.img"
Write-Host "QEMU window should open now!" -ForegroundColor Green
