/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <windows.h>
#include <vector>
#include "Deploy.h"
#include "FileGuard.h"
#include "RegistryKey.h"
#include "ErrorUtils.h"
#include "GetExeFileData.h"

namespace Deploy
{

void Install(const std::wstring &InstallPath)
{
    const std::wstring keyName = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask";

    if(Utils::RegistryKey::IsExists(HKEY_LOCAL_MACHINE, keyName))
        throw AlreadyInstalledException(L"NetboxTask is already installed");

    //copy exe file
    std::wstring exePath;
    std::wstring exeName;

    Utils::GetExeFileData(exePath, exeName);

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

    //make installed app registry entries
    {
        Utils::RegistryKey key = {HKEY_LOCAL_MACHINE, keyName, Utils::RegistryKey::MODE_CREATE};

        key.WriteString(L"UninstallString", L"\"" + exeDst + L"\" -u");
        key.WriteString(L"DisplayName",     L"Netbox test task");
        key.WriteString(L"Publisher",       L"Alexey Frolov(alexwin32@mail.ru)");
        key.WriteString(L"InstallLocation", InstallPath);
    }

    //create TestFolder registry entry
    {
        Utils::RegistryKey key1 = {HKEY_LOCAL_MACHINE, L"Software\\TestFolder", Utils::RegistryKey::MODE_CREATE};
        Utils::RegistryKey key2 = {HKEY_LOCAL_MACHINE, L"Software\\TestFolder\\NetboxTestTask", Utils::RegistryKey::MODE_CREATE};
    }
}

void Uninstall()
{
    const std::wstring keyName = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask";

    if(!Utils::RegistryKey::IsExists(HKEY_LOCAL_MACHINE, keyName))
        throw NotInstalledException(L"NetboxTask is not installed");

    std::wstring exePath;

    {
        Utils::RegistryKey key = {HKEY_LOCAL_MACHINE, keyName, Utils::RegistryKey::MODE_OPEN};

        std::wstring installLocation = key.ReadString(L"InstallLocation");

        exePath = installLocation + L"\\NetboxTask.exe";
    }

    MoveFileExW(exePath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);

    Utils::RegistryKey::Delete(HKEY_LOCAL_MACHINE, keyName);
    Utils::RegistryKey::Delete(HKEY_LOCAL_MACHINE, L"Software\\TestFolder\\NetboxTestTask");
    Utils::RegistryKey::Delete(HKEY_LOCAL_MACHINE, L"Software\\TestFolder");
    
}

}