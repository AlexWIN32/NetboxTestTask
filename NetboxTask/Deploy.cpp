/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <windows.h>
#include <vector>
#include "Deploy.h"
#include "FileGuard.h"
#include "ErrorUtils.h"

namespace Deploy
{

static void GetExeFileData(std::wstring &Path, std::wstring &Name)
{
    WCHAR path[MAX_PATH + 1] = {};

    GetModuleFileNameW(nullptr, path, MAX_PATH + 1);

    std::wstring pathStr = path;

    Path = pathStr.substr(0, pathStr.find_last_of('\\'));
    Name = pathStr.substr(pathStr.find_last_of('\\') + 1);
}

static void WriteStringToRegistry(HKEY Key, const std::wstring &VarName, const std::wstring &VarValue)
{
    LSTATUS res = RegSetKeyValueW(Key,
                                  nullptr,
                                  VarName.c_str(),
                                  REG_SZ,
                                  VarValue.c_str(),
                                  (VarValue.size() + 1) * sizeof(wchar_t));
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant write " + VarName + L" registry value:" + Utils::GetErrorMessageString(res));
}

static std::wstring ReadStringFromRegistry(HKEY Key, const std::wstring &VarName)
{
    DWORD dataSize;
    LSTATUS res = RegQueryValueExW(Key,
                                   L"InstallLocation",
                                   0,
                                   nullptr,
                                   nullptr,
                                   &dataSize);
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant read " + VarName + L" registry value:" + Utils::GetErrorMessageString(res));

    std::vector<wchar_t> rawData(dataSize / sizeof(wchar_t));

    res = RegQueryValueExW(Key,
                           L"InstallLocation",
                           0,
                           nullptr,
                           reinterpret_cast<LPBYTE>(&rawData[0]),
                           &dataSize);
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant read " + VarName + L" registry value:" + Utils::GetErrorMessageString(res));

    return &rawData[0];
}

void Install(const std::wstring &InstallPath)
{
    //check weather the application is installed or not
    HKEY hk;
    LSTATUS res = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask",
                                0,
                                KEY_QUERY_VALUE,
                                &hk);
    if(res == ERROR_SUCCESS)
        throw AlreadyInstalledException(L"NetboxTask is already installed");

    //copy exe file
    std::wstring exePath;
    std::wstring exeName;

    GetExeFileData(exePath, exeName);

    CreateDirectoryW(InstallPath.c_str(), nullptr);

    std::wstring exeDst = InstallPath + L"\\" + exeName;

    Utils::FileGuard from = {exePath + L"\\" + exeName, L"r"};
    Utils::FileGuard to = {exeDst, L"w+"};

    char *dataChunk[256];
    char *ptr = reinterpret_cast<char*>(dataChunk);

    while(true)
    {
        size_t readed = from.Read<char>(ptr, 256);

        if(readed == 0)
            break;

        to.Write<char>(ptr, readed);
    }

    //make registry entries
    res = RegCreateKeyExW(HKEY_LOCAL_MACHINE,
                          L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask",
                          0,
                          nullptr,
                          REG_OPTION_NON_VOLATILE,
                          KEY_WRITE | KEY_WOW64_32KEY,
                          nullptr,
                          &hk,
                          nullptr);
    if(res != ERROR_SUCCESS)
    {
        std::wstring msg = Utils::GetErrorMessageString(res);
        throw RegistryOperationException(L"cant create NetboxTask registry entry:" + Utils::GetErrorMessageString(res));
    }

    WriteStringToRegistry(hk, L"UninstallString", L"\"" + exeDst + L"\" -u");
    WriteStringToRegistry(hk, L"DisplayName", L"Netbox test task");
    WriteStringToRegistry(hk, L"Publisher", L"Alexey Frolov(alexwin32@mail.ru)");
    WriteStringToRegistry(hk, L"InstallLocation", InstallPath);

    RegCloseKey(hk);
}

void Uninstall()
{
    HKEY hk;
    LSTATUS res = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask",
                                0,
                                KEY_QUERY_VALUE,
                                &hk);
    if(res != ERROR_SUCCESS)
        throw AlreadyInstalledException(L"NetboxTask is not installed");

    std::wstring installLocation = ReadStringFromRegistry(hk, L"InstallLocation");

    std::wstring exePath = installLocation + L"\\NetboxTask.exe";

    /*
    if(!DeleteFileW(exePath.c_str()))
    {
        int err = GetLastError();

        if(err != ERROR_PATH_NOT_FOUND && err != ERROR_FILE_NOT_FOUND)
            throw IOException(L"cant delete " + exePath);
    }
    
    RemoveDirectoryW(installLocation.c_str());
    */

    MoveFileExW(exePath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);

    RegCloseKey(hk);

    res = RegDeleteKeyW(HKEY_LOCAL_MACHINE,
                        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask");
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant delete NetboxTask registry entry:" + Utils::GetErrorMessageString(res));
}

}