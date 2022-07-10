Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

. (Join-Path -Path $PSScriptRoot -ChildPath "Common.ps1")

Function Add-TextExpansion($trigger, $replacement)
{
    $path = Join-Path -Path (Join-Path -Path ([Environment]::GetFolderPath("ApplicationData")) -ChildPath $ApplicationName) -ChildPath $TextExpansionsFileName
    $expansions = $null
    try
    {
        $expansions = Parse-JsonFile $path
        if (!($expansions -is [array]))
        {
            Write-File $path "[]"
            $expansions = Parse-JsonFile $path
        }
    }
    catch [ArgumentException]
    {
        Write-File $path "[]"
        $expansions = Parse-JsonFile $path
    }

    $object = @{
        "trigger" = $trigger
        "replacement" = $replacement
    }
    $expansions += $object
    ConvertTo-Json $expansions -Depth 100 | Out-File -FilePath $path -Encoding UTF8

    # Rewrites to file to remove BOM
    Write-File $path (Get-Content $path -Raw)
}

Function Main
{
    $formSizeX = 500
    $formSizeY = 400

    $form = New-Object System.Windows.Forms.Form
    $form.AutoScroll = $true
    $form.Size = New-Object System.Drawing.Size($formSizeX,$formSizeY)
    $form.StartPosition = "CenterScreen"
    $form.Text = "Create a Text Expansion"
    $form.Topmost = $true

    $okButton = New-Object System.Windows.Forms.Button
    $okButton.DialogResult = [System.Windows.Forms.DialogResult]::OK
    $okButton.Enabled = $false
    $okButton.Location = New-Object System.Drawing.Point(75,($formSizeY - 75))
    $okButton.Size = New-Object System.Drawing.Size(75,23)
    $okButton.Text = "OK"
    $form.AcceptButton = $okButton
    $form.Controls.Add($okButton)

    $cancelButton = New-Object System.Windows.Forms.Button
    $cancelButton.DialogResult = [System.Windows.Forms.DialogResult]::Cancel
    $cancelButton.Location = New-Object System.Drawing.Point(150,($formSizeY - 75))
    $cancelButton.Size = New-Object System.Drawing.Size(75,23)
    $cancelButton.Text = "Cancel"
    $form.CancelButton = $cancelButton
    $form.Controls.Add($cancelButton)

    $label = New-Label "Trigger:" ($formSizeX - 50) 10 20
    $form.Controls.Add($label)
    $triggerTextBox = New-TriggerTextBox ($formSizeX - 50) 10 40
    $form.Controls.Add($triggerTextBox)
    
    $label = New-Label "Replacement:" ($formSizeX - 50) 10 80
    $form.Controls.Add($label)
    $replacementTextBox = New-ReplacementTextBox ($formSizeX - 50) 10 100
    $form.Controls.Add($replacementTextBox)

    $triggerTextBox.Add_TextChanged({$okButton.Enabled = if ($This.Text) {$true} else {$false}})

    $form.Add_Shown({$triggerTextBox.Select()})
    $result = $form.ShowDialog()
    if ($result -eq [System.Windows.Forms.DialogResult]::OK)
    {
        Add-TextExpansion $triggerTextBox.Text $replacementTextBox.Text
    }
}

Main
