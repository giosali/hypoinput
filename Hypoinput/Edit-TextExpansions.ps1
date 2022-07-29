Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

. (Join-Path -Path $PSScriptRoot -ChildPath "Common.ps1")

$global:Resource = @{}

Function New-Button($text, $dialogResult, $pointX, $pointY)
{
    $button = New-Object System.Windows.Forms.Button
    $button.DialogResult = $dialogResult
    $button.Location = New-Object System.Drawing.Point($pointX,$pointY)
    $button.Size = New-Object System.Drawing.Size(75,23)
    $button.Text = $text
    return $button
}

Function UpdateResource($locale)
{
    $formText
    $listBoxLabel
    $trigger
    $replacement

    $saveButtonText
    $cancelButtonText
    $deleteButtonText

    Switch ($locale)
    {
        0
        {
            $formText = "Edit Text Expansions"
            $listBoxLabel = "Your text expansions:"
            $trigger = "Trigger:"
            $replacement = "Replacement:"
            $saveButtonText = "Save"
            $cancelButtonText = "Cancel"
            $deleteButtonText = "Delete"
            break
        }
        1 
        {
            $formText = "Editar sus expansiónes de texto"
            $listBoxLabel = "Sus expansiónes de texto:"
            $trigger = "Disparador:"
            $replacement = "Reemplazo:"
            $saveButtonText = "Guardar"
            $cancelButtonText = "Cancelar"
            $deleteButtonText = "Borrar"
            break
        }
        2
        {
            $formText = "Modifier vos expansions de texte"
            $listBoxLabel = "Vos expansions de texte"
            $trigger = "Déclencheur:"
            $replacement = "Remplacement:"
            $saveButtonText = "Enregistrer"
            $cancelButtonText = "Annuler";
            $deleteButtonText = "Supprimer"
            break
        }
    }

    $Resource["formText"] = $formText
    $Resource["listBoxLabel"] = $listBoxLabel
    $Resource["trigger"] = $trigger
    $Resource["replacement"] = $replacement
    $Resource["saveButtonText"] = $saveButtonText
    $Resource["cancelButtonText"] = $cancelButtonText
    $Resource["deleteButtonText"] = $deleteButtonText
}

Function Main($locale)
{
    UpdateResource($locale)

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

    $formSizeX = 600
    $formSizeY = 500
    $pointX = 10
    $pointY = 0

    # Form
    $form = New-Object System.Windows.Forms.Form
    $form.AutoScroll = $true
    $form.Size = New-Object System.Drawing.Size($formSizeX,$formSizeY)
    $form.StartPosition = "CenterScreen"
    $form.Text = $Resource["formText"];
    $form.Topmost = $true

    # ListBox
    $form.Controls.Add((New-Label $Resource["listBoxLabel"] ($formSizeX - 50) $pointX ($pointY += 20)))
    $listBox = New-Object System.Windows.Forms.ListBox
    $listBox.DisplayMember = "trigger"
    $listBox.Location = New-Object System.Drawing.Point($pointX,(($pointY += 20 + 150) - 150))
    $listBox.Size = New-Object System.Drawing.Size(($formSizeX - 50),150)
    foreach ($expansion in $expansions)
    {
        # `[void]` prevents output.
        [void] $listBox.Items.Add($expansion)
    }

    Function Write-Expansions
    {
        $array = ConvertFrom-Json "[]"
        foreach ($expansion in $listBox.Items)
        {
            $array += $expansion
        }
        
        ConvertTo-Json $array -Depth 100 | Out-File -FilePath $path -Encoding UTF8

        # Rewrites to file to remove BOM
        Write-File $path (Get-Content $path -Raw)
    }

    $form.Controls.Add($listBox)

    # TextBoxes
    $triggerLabel = New-Label $Resource["trigger"] ($formSizeX - 50) $pointX ($pointY += 20)
    $triggerLabel.Enabled = $false
    $form.Controls.Add($triggerLabel)
    $triggerTextBox = New-TriggerTextBox ($formSizeX - 50) $pointX ($pointY += 20)
    $triggerTextBox.Enabled = $false
    $form.Controls.Add($triggerTextBox)

    $replacementLabel = New-Label $Resource["replacement"] ($formSizeX - 50) $pointX ($pointY += 40)
    $replacementLabel.Enabled = $false
    $form.Controls.Add($replacementLabel)
    $replacementTextBox = New-ReplacementTextBox ($formSizeX - 50) $pointX (($pointY += 20 + 100) - 100)
    $replacementTextBox.Enabled = $false
    $form.Controls.Add($replacementTextBox)

    # Buttons
    $saveButton = New-Button $Resource["saveButtonText"] ([System.Windows.Forms.DialogResult]::None) ($formSizeX - 40 - 225) ($pointY += 30)
    $form.Controls.Add($saveButton)

    $cancelButton = New-Button $Resource["cancelButtonText"] ([System.Windows.Forms.DialogResult]::Cancel) ($formSizeX - 40 - 150) $pointY
    $form.CancelButton = $cancelButton
    $form.Controls.Add($cancelButton)

    $deleteButton = New-Button $Resource["deleteButtonText"] ([System.Windows.Forms.DialogResult]::None) ($formSizeX - 40 - 75) $pointY
    $deleteButton.Enabled = $false
    $form.Controls.Add($deleteButton)

    # Event Handling
    $listBox.Add_SelectedValueChanged({
        $replacementLabel.Enabled = $replacementTextBox.Enabled = $triggerLabel.Enabled = $triggerTextBox.Enabled = !($listBox.SelectedItem -eq $null)

        $triggerTextBox.Text = $listBox.SelectedItem.trigger
        $replacementTextBox.Text = $listBox.SelectedItem.replacement

        $deleteButton.Enabled = !($listBox.SelectedItem -eq $null)
    })
    $triggerTextBox.Add_TextChanged({$saveButton.Enabled = if ($This.Text) {$true} else {$false}})
    $saveButton.Add_Click({
        $selectedExpansion = $listBox.SelectedItem
        if ($selectedExpansion -eq $null)
        {
            return
        }
        
        $selectedExpansion.trigger = $triggerTextBox.Text
        $selectedExpansion.replacement = $replacementTextBox.Text
        $listBox.Items[$listBox.SelectedIndex] = $selectedExpansion
        Write-Expansions
    })
    $deleteButton.Add_Click({
        if ($listBox.SelectedItem -eq $null)
        {
            return
        }

        $listBox.Items.Remove($listBox.SelectedItem)
        if ($listBox.Items.Count -gt 0)
        {
            Write-Expansions
        }
        else
        {
            Write-File $path "[]"
        }
    })
    

    $result = $form.ShowDialog()
    if ($result -eq [System.Windows.Forms.DialogResult]::Cancel)
    {
        return
    }
}

Main $args[0]
