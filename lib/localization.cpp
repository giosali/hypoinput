#include "localization.h"

namespace localization {

Resource::Resource()
    : m_locale()
{
}

Resource::Resource(Locale locale)
    : m_locale(locale)
{
    std::wstring disable;
    std::wstring enable;
    std::wstring runAtStartup;
    std::wstring openFile;
    std::wstring addTextExpansion;
    std::wstring editTextExpansions;
    std::wstring checkForUpdates;
    std::wstring exit;

    std::wstring updatesAvailableText;
    std::wstring updatesAvailableCaption;
    std::wstring noUpdatesAvailableText;
    std::wstring noUpdatesAvailableCaption;

    switch (locale) {
    case Locale::EN:
        disable = L"Disable";
        enable = L"Enable";
        runAtStartup = L"Run at startup";
        openFile = L"Open file";
        addTextExpansion = L"Add text expansion";
        editTextExpansions = L"Edit text expansions";
        checkForUpdates = L"Check for updates";
        exit = L"Exit";

        updatesAvailableText = L"A new update is available! Would you like to update?";
        updatesAvailableCaption = L"Update Available";
        noUpdatesAvailableText = L"You're currently running the latest version! Check back again later for an update.";
        noUpdatesAvailableCaption = L"No Updates Available";
        break;
    case Locale::ES:
        disable = L"Desactivar";
        enable = L"Activar";
        runAtStartup = L"Ejecutar al inicio";
        openFile = L"Abrir archivo";
        addTextExpansion = L"Agregar una expansión de texto";
        editTextExpansions = L"Editar sus expansiónes de texto";
        checkForUpdates = L"Buscar actualizaciones";
        exit = L"Cerrar";

        updatesAvailableText = L"¡Hay una nueva actualización! ¿Te gustaría actualizar?";
        updatesAvailableCaption = L"actualización disponible";
        noUpdatesAvailableText = L"¡Actualmente estás usando la última versión! Vuelva a comprobar si hay actualizaciones disponibles mas luego.";
        noUpdatesAvailableCaption = L"No hay actualizaciones disponibles";
        break;
    case Locale::FR:
        disable = L"Désactiver";
        enable = L"Activer";
        runAtStartup = L"Lancer au démarrage";
        openFile = L"Ouvrir le fichier";
        addTextExpansion = L"Ajouter une expansion de texte";
        editTextExpansions = L"Modifier vos expansions de texte";
        checkForUpdates = L"Rechercher des mises à jour";
        exit = L"Quitter";

        updatesAvailableText = L"Une nouvelle mise à jour est disponible! Voulez-vous mettre à jour ce logiciel?";
        updatesAvailableCaption = L"Mises à jour disponible";
        noUpdatesAvailableText = L"Vous utilisez actuellement la dernière version! Revenez plus tard pour rechercher des mises à jour.";
        noUpdatesAvailableCaption = L"Aucune mise à jour disponible";
        break;
    }

    m_textResources[Text::Disable] = disable;
    m_textResources[Text::Enable] = enable;
    m_textResources[Text::RunAtStartup] = runAtStartup;
    m_textResources[Text::OpenFile] = openFile;
    m_textResources[Text::AddTextExpansion] = addTextExpansion;
    m_textResources[Text::EditTextExpansions] = editTextExpansions;
    m_textResources[Text::CheckForUpdates] = checkForUpdates;
    m_textResources[Text::Exit] = exit;

    m_textResources[Text::UpdatesAvailableText] = updatesAvailableText;
    m_textResources[Text::UpdatesAvailableCaption] = updatesAvailableCaption;
    m_textResources[Text::NoUpdatesAvailableText] = noUpdatesAvailableText;
    m_textResources[Text::NoUpdatesAvailableCaption] = noUpdatesAvailableCaption;
}

const std::wstring& Resource::operator[](Text textResource)
{
    return m_textResources[textResource];
}

} // namespace localization