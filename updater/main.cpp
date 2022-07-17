#include "environment.h"
#include "picojson.h"
#include <Windows.h>
#include <cpr/cpr.h>
#include <filesystem>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    const std::string latestReleaseEp = "https://api.github.com/repos/giosali/hypoinput/releases/latest";
    cpr::Response r = cpr::Get(cpr::Url { latestReleaseEp });
    if (r.status_code != cpr::status::HTTP_OK && r.status_code >= 400) {
        MessageBox(NULL, L"Received 400-level status code while trying to get latest release", NULL, MB_OK);
        return 1;
    }

    try {
        picojson::value v;
        std::string err = picojson::parse(v, r.text);
        if (!err.empty()) {
            MessageBox(NULL, L"Could not parse JSON from latest release", NULL, MB_OK);
            return 1;
        }

        if (!v.is<picojson::object>()) {
            MessageBox(NULL, L"JSON is not an object", NULL, MB_OK);
            return 1;
        }

        picojson::array& assets = v.get("assets").get<picojson::array>();
        if (assets.size() == 0) {
            MessageBox(NULL, L"No assets were found", NULL, MB_OK);
            return 1;
        }

        std::filesystem::path tmpPath = environment::getFolderPath(environment::SpecialFolder::TempHypoinputApplicationData);
        std::filesystem::path installerPath = tmpPath / "installer.msi";
        std::filesystem::path outputPath = tmpPath / "output";
        std::filesystem::create_directories(tmpPath);

        // Gets the URL for the installer file and then downloads and saves
        // it to AppData.
        picojson::object& asset = assets[0].get<picojson::object>();
        std::ofstream ofs(installerPath, std::ios::binary);
        cpr::Session s = cpr::Session();
        s.SetUrl(cpr::Url { asset["browser_download_url"].get<std::string>() });
        r = s.Download(ofs);
        ofs.close();

        // Extracts the contents of the installer file.
        std::wstring parameters = L"/a " + installerPath.wstring() + L" /qn TARGETDIR=" + outputPath.wstring();
        SHELLEXECUTEINFO shExecInfo = { sizeof(SHELLEXECUTEINFO) };
        shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExecInfo.hwnd = NULL;
        shExecInfo.lpVerb = NULL;
        shExecInfo.lpFile = L"msiexec";
        shExecInfo.lpParameters = parameters.c_str();
        shExecInfo.lpDirectory = NULL;
        shExecInfo.nShow = SW_SHOW;
        shExecInfo.hInstApp = NULL;
        ShellExecuteEx(&shExecInfo);
        if (shExecInfo.hProcess != NULL) {
            WaitForSingleObject(shExecInfo.hProcess, INFINITE);
            CloseHandle(shExecInfo.hProcess);
        }

        // Changes the name of the current application executable located in the
        // current user's Program Files.
        std::filesystem::rename(environment::getFilePath(environment::SpecialFile::ApplicationExecutable), environment::getFilePath(environment::SpecialFile::OldApplicationExecutable));

        std::filesystem::path executableDir = environment::getFolderPath(environment::SpecialFolder::Executable);
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(outputPath)) {
            std::filesystem::path p = entry.path();

            // Skips the smaller installer file that appears after extraction by msiexec.
            if (p.has_extension() && p.extension() == ".msi") {
                continue;
            }

            // Moves every file/directory in the output directory to Program Files.
            std::filesystem::rename(p, executableDir / p.filename());
        }

        // Deletes the tmp directory and all of its contents.
        std::filesystem::remove_all(tmpPath);

        // "Restarts" application.
        ShellExecute(NULL, NULL, (environment::getFilePath(environment::SpecialFile::ApplicationExecutable)).wstring().c_str(), NULL, NULL, SW_SHOW);

        // Removes the old executable.
        std::filesystem::remove(environment::getFilePath(environment::SpecialFile::OldApplicationExecutable));
    } catch (std::runtime_error) {
        //bool tmpPathExists = 
        MessageBox(NULL, L"Encountered a runtime error! Please file an issue at:\nhttps://github.com/giosali/hypoinput/issues", NULL, MB_OK);
        return 1;
    }

    return 0;
}