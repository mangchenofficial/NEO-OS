$size = 1474560
$img = New-Object byte[] $size
[System.IO.File]::WriteAllBytes('os.img', $img)
$boot = [System.IO.File]::ReadAllBytes('boot.bin')
$kern = [System.IO.File]::ReadAllBytes('kernel.bin')
$stream = [System.IO.File]::OpenWrite('os.img')
$stream.Write($boot, 0, $boot.Length)
$stream.Position = 512
$stream.Write($kern, 0, $kern.Length)
$stream.Close()
Write-Host 'Image created successfully!'
