@echo off
set PATH=C:\Program Files\qemu;%PATH%
echo ======================================
echo   NeoOS QEMU Launcher
echo ======================================
echo.

if not exist "os.img" (
    echo ERROR: os.img not found!
    echo Please run "build.bat" first.
    pause
    exit /b 1
)

echo Launching NeoOS in QEMU...
echo.

powershell -Command "Start-Process -FilePath 'qemu-system-i386.exe' -ArgumentList '-drive','format=raw,file=os.img'"
