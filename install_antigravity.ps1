Write-Host "==============================="
Write-Host "Closing Antigravity processes..."
Write-Host "==============================="

$processNames = @(
    "antigravity",
    "google-antigravity",
    "Antigravity",
    "winget"
)

foreach ($name in $processNames) {
    Get-Process -Name $name -ErrorAction SilentlyContinue |
        Where-Object { $_.StartInfo.EnvironmentVariables["USERNAME"] -eq $env:USERNAME } |
        Stop-Process -Force -ErrorAction SilentlyContinue
}

Start-Sleep -Seconds 3

Write-Host "==============================="
Write-Host "Installing Google Antigravity (user mode)..."
Write-Host "==============================="

winget install --id Google.Antigravity `
    --scope user `
    --silent `
    --accept-package-agreements `
    --accept-source-agreements

Write-Host ""
Write-Host "Done."
Pause