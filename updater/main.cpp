#include "environment.h"
#include "picojson.h"
#include "utils.h"
#include <Windows.h>
#include <cpr/cpr.h>
#include <filesystem>
#include <iterator>
#include <system_error>

// Forward declarations

std::filesystem::path append(const std::filesystem::path&, const std::string_view&);

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

        std::filesystem::path tempDir = environment::getFolderPath(environment::SpecialFolder::HypoinputTemp);
        std::filesystem::create_directories(tempDir);
        std::filesystem::path installerFile = tempDir / "installer.msi";
        std::filesystem::path outputDir = tempDir / "output";

        // Gets the URL for the installer file and then downloads and saves it to %TEMP%
        picojson::object& asset = assets[0].get<picojson::object>();
        std::ofstream ofs(installerFile, std::ios::binary);
        cpr::Session s = cpr::Session();
        s.SetUrl(cpr::Url { asset["browser_download_url"].get<std::string>() });
        r = s.Download(ofs);
        ofs.close();

        // Extracts the contents of the installer file.
        std::wstring parameters = L"/a " + installerFile.wstring() + L" /qn TARGETDIR=" + outputDir.wstring();
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
        std::error_code errorCode;
        std::filesystem::path exeFile = environment::getFilePath(environment::SpecialFile::ApplicationExecutable);
        std::filesystem::rename(exeFile, append(exeFile, environment::constants::oldSuffix), errorCode);
        if (errorCode) {
            MessageBox(NULL, utils::stringToWString(errorCode.message()).c_str(), L"Rename Application Executable Failed", MB_OK);
            errorCode.clear();
        }

        std::filesystem::path exeDir = environment::getFolderPath(environment::SpecialFolder::Executable);
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(outputDir)) {
            std::filesystem::path outputItem = entry.path();

            // Skips the smaller installer file that appears after extraction by msiexec.
            if (outputItem.has_extension() && outputItem.extension() == ".msi") {
                continue;
            }

            // Skips directories.
            if (std::filesystem::is_directory(outputItem)) {
                continue;
            }

            // Attempts to move file from output directory to ProgramFiles
            std::filesystem::path exeItem = exeDir / outputItem.filename();
            std::filesystem::rename(outputItem, exeItem, errorCode);
            if (!errorCode) {
                continue;
            }

            if (errorCode.value() != ERROR_ACCESS_DENIED) {
                MessageBox(NULL, utils::stringToWString(errorCode.message()).c_str(), L"Move Output Item Failed", MB_OK);
                continue;
            }

            // Handles instance where the file already exists and is currently in use.
            std::filesystem::path oldExeItem = exeDir / append(outputItem, environment::constants::oldSuffix).stem();
            std::filesystem::rename(exeItem, oldExeItem, errorCode);
            if (errorCode) {
                MessageBox(NULL, utils::stringToWString(errorCode.message()).c_str(), L"Rename Executable Directory Item Failed", MB_OK);
                errorCode.clear();
                continue;
            }

            // .old files will be deleted when the user reboots their computer.
            MoveFileEx(oldExeItem.wstring().c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

            // Retry moving the file from the output directory.
            std::filesystem::rename(outputItem, exeItem, errorCode);
            if (errorCode) {
                MessageBox(NULL, utils::stringToWString(errorCode.message()).c_str(), L"Retry Rename Ouput Item Failed", MB_OK);
            }
        }

        // Deletes the tmp directory and all of its contents.
        std::filesystem::remove_all(tempDir, errorCode);
        if (errorCode) {
            MessageBox(NULL, utils::stringToWString(errorCode.message()).c_str(), L"Remove Output Directory Failed", MB_OK);
            errorCode.clear();
        }

        // "Restarts" application.
        ShellExecute(NULL, NULL, (environment::getFilePath(environment::SpecialFile::ApplicationExecutable)).wstring().c_str(), NULL, NULL, SW_SHOW);

        // Removes the old executable.
        std::filesystem::remove(append(exeFile, environment::constants::oldSuffix), errorCode);
        if (errorCode) {
            MessageBox(NULL, utils::stringToWString(errorCode.message()).c_str(), L"Remove Old Executable Failed", MB_OK);
        }
    } catch (std::runtime_error) {
        MessageBox(NULL, L"Encountered a runtime error! Please file an issue at:\nhttps://github.com/giosali/hypoinput/issues", NULL, MB_OK);
        return 1;
    }

    return 0;
}

std::filesystem::path append(const std::filesystem::path& path, const std::string_view& text)
{
    return path.parent_path() / (path.stem().string() + std::string(text) + path.extension().string());
}