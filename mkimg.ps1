$size = 1474560
$img = New-Object byte[] $size
[System.IO.File]::WriteAllBytes('os.img', $img)
$boot = [System.IO.File]::ReadAllBytes('boot.bin')
$kern = [System.IO.File]::ReadAllBytes('kernel.bin')
$s = [System.IO.File]::OpenWrite('os.img')
$s.Write($boot, 0, $boot.Length)
$s.Position = 512
$s.Write($kern, 0, $kern.Length)
$s.Close()
Write-Host 'Image created'
