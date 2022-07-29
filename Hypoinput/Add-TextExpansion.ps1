Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

. (Join-Path -Path $PSScriptRoot -ChildPath "Common.ps1")

$global:Resource = @{}

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

Function UpdateResource($locale)
{
    $formText
    $cancelButtonText
    $trigger
    $replacement

    Switch ($locale)
    {
        0
        {
            $formText = "Create a Text Expansion"
            $cancelButtonText = "Cancel"
            $trigger = "Trigger:"
            $replacement = "Replacement:"
            break
        }
        1 
        {
            $formText = "Crear una expansión de texto"
            $cancelButtonText = "Cancelar"
            $trigger = "Disparador:"
            $replacement = "Reemplazo:"
            break
        }
        2
        {
            $formText = "Créer une expansion de texte"
            $cancelButtonText = "Annuler";
            $trigger = "Déclencheur:"
            $replacement = "Remplacement:"
            break
        }
    }

    $Resource["formText"] = $formText
    $Resource["cancelButtonText"] = $cancelButtonText
    $Resource["trigger"] = $trigger
    $Resource["replacement"] = $replacement
}

Function Main($locale)
{
    UpdateResource $locale

    $formSizeX = 500
    $formSizeY = 400

    # Form
    $form = New-Object System.Windows.Forms.Form
    $form.AutoScroll = $true
    $form.Size = New-Object System.Drawing.Size($formSizeX,$formSizeY)
    $form.StartPosition = "CenterScreen"
    $form.Text = $Resource["formText"]
    $form.Topmost = $true

    # Buttons
    $okButton = New-Button "OK" ([System.Windows.Forms.DialogResult]::OK) ($formSizeX - 40 - 150) ($formSizeY - 75)
    $okButton.Enabled = $false
    $form.AcceptButton = $okButton
    $form.Controls.Add($okButton)

    $cancelButton = New-Button $Resource["cancelButtonText"] ([System.Windows.Forms.DialogResult]::Cancel) ($formSizeX - 40 - 75) ($formSizeY - 75)
    $form.CancelButton = $cancelButton
    $form.Controls.Add($cancelButton)

    #TextBoxes
    $label = New-Label $Resource["trigger"] ($formSizeX - 50) 10 20
    $form.Controls.Add($label)
    $triggerTextBox = New-TriggerTextBox ($formSizeX - 50) 10 40
    $form.Controls.Add($triggerTextBox)
    
    $label = New-Label $Resource["replacement"] ($formSizeX - 50) 10 80
    $form.Controls.Add($label)
    $replacementTextBox = New-ReplacementTextBox ($formSizeX - 50) 10 100
    $form.Controls.Add($replacementTextBox)

    # Event Handling
    $triggerTextBox.Add_TextChanged({$okButton.Enabled = if ($This.Text) {$true} else {$false}})

    $form.Add_Shown({$triggerTextBox.Select()})
    $result = $form.ShowDialog()
    if ($result -eq [System.Windows.Forms.DialogResult]::OK)
    {
        Add-TextExpansion $triggerTextBox.Text $replacementTextBox.Text
    }
}

Main $args[0]
