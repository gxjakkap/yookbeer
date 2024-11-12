if (-not (Test-Path -Path "./data")){
    $null = New-Item -Path "./data" -ItemType "directory"
}


$sourcePath = ".\data_template\data.csv"
$destinationPath = ".\data\data.csv"


Copy-Item $sourcePath -Destination $destinationPath -Force
Write-Host "Test data copied to $destinationPath"
