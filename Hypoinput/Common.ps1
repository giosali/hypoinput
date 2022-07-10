$global:ApplicationName = "Hypoinput"
$global:TextExpansionsFileName = "TextExpansions.json"

Function Parse-JsonFile($path)
{
    return Get-Content $path -Encoding UTF8 | ConvertFrom-Json
}

Function Write-File($path, $content)
{
    # Removes BOM.
    $utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllLines($path, $content, $utf8NoBomEncoding)
}

Function New-Button($text, $dialogResult, $pointX, $pointY)
{
    $button = New-Object System.Windows.Forms.Button
    $button.DialogResult = $dialogResult
    $button.Location = New-Object System.Drawing.Point($pointX,$pointY)
    $button.Size = New-Object System.Drawing.Size(75,23)
    $button.Text = $text
    return $button
}

Function New-Label($text, $sizeX, $pointX, $pointY)
{
    $label = New-Object System.Windows.Forms.Label
    $label.Location = New-Object System.Drawing.Point($pointX,$pointY)
    $label.Size = New-Object System.Drawing.Size($sizeX,20)
    $label.Text = $text
    return $label
}

Function New-TriggerTextBox($sizeX, $pointX, $pointY)
{
    $textBox = New-Object System.Windows.Forms.TextBox
    $textBox.Location = New-Object System.Drawing.Point($pointX,$pointY)
    $textBox.ShortcutsEnabled = $true
    $textBox.Size = New-Object System.Drawing.Size($sizeX,20)
    return $textBox
}

Function New-ReplacementTextBox($sizeX, $pointX, $pointY)
{
    $textBox = New-Object System.Windows.Forms.TextBox
    $textBox.AcceptsReturn = $true
    $textBox.AcceptsTab = $true
    $textBox.Location = New-Object System.Drawing.Point($pointX,$pointY)
    $textBox.Multiline = $true
    $textBox.ScrollBars = "Vertical"
    $textBox.ShortcutsEnabled = $true
    $textBox.Size = New-Object System.Drawing.Size($sizeX,100)
    return $textBox
}

