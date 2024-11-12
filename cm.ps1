$dir = "./src"

if (-not (Test-Path -Path "./bin")){
    $null = New-Item -Path "./bin" -ItemType "directory"
}

if (Test-Path -Path "$dir/main.c" -PathType Leaf){
    $compileCommand = "gcc `"$($dir)\main.c`" -o `".\bin\yookbeer.exe`""
    Write-Host "Compiling with command: $compileCommand"
    
    Invoke-Expression -Command $compileCommand -ErrorAction Stop

    if ($LASTEXITCODE -ne 0) {
        throw "Error compiling. Check the source code for errors."
    }
    else {
        Write-Host "Yookbeer compiled successfully! binary at bin/yookbeer.exe"
    }
}
else {
    Write-Host "File does not exist!"
}
