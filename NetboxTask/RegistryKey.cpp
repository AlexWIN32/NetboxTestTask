/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/
#include "RegistryKey.h"
#include "ErrorUtils.h"
#include <vector>

namespace Utils
{

RegistryKey::RegistryKey(HKEY Key, const std::wstring &SubKey, Operation Op)
{
    LSTATUS res;
    if(Op == MODE_OPEN)
    {
        res = RegOpenKeyExW(Key,
                            SubKey.c_str(),
                            0,
                            KEY_QUERY_VALUE,
                            &key);
    }
    else if(Op == MODE_CREATE)
    {
        res = RegCreateKeyExW(Key,
                              SubKey.c_str(),
                              0,
                              nullptr,
                              REG_OPTION_NON_VOLATILE,
                              KEY_WRITE | KEY_WOW64_32KEY,
                              nullptr,
                              &key,
                              nullptr);
    }

    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"registry key " + SubKey + L" processing error:" + Utils::GetErrorMessageString(res));
}

RegistryKey::~RegistryKey()
{
    if(key != nullptr)
        RegCloseKey(key);
}

std::wstring RegistryKey::ReadString(const std::wstring &VarName) const
{
    DWORD dataSize;
    LSTATUS res = RegQueryValueExW(key,
                                   L"InstallLocation",
                                   0,
                                   nullptr,
                                   nullptr,
                                   &dataSize);
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant read " + VarName + L" registry value:" + Utils::GetErrorMessageString(res));

    std::vector<wchar_t> rawData(dataSize / sizeof(wchar_t));

    res = RegQueryValueExW(key,
                           L"InstallLocation",
                           0,
                           nullptr,
                           reinterpret_cast<LPBYTE>(&rawData[0]),
                           &dataSize);
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant read " + VarName + L" registry value:" + Utils::GetErrorMessageString(res));

    return &rawData[0];
}

void RegistryKey::WriteString(const std::wstring &VarName, const std::wstring &VarValue)
{
    LSTATUS res = RegSetKeyValueW(key,
                                  nullptr,
                                  VarName.c_str(),
                                  REG_SZ,
                                  VarValue.c_str(),
                                  (VarValue.size() + 1) * sizeof(wchar_t));
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant write " + VarName + L" registry value:" + Utils::GetErrorMessageString(res));
}

void RegistryKey::Delete(HKEY Key, const std::wstring &SubKey)
{
    LSTATUS res = RegDeleteKeyW(Key, SubKey.c_str());
    if(res != ERROR_SUCCESS)
        throw RegistryOperationException(L"cant delete registry " + SubKey + L" key:" + Utils::GetErrorMessageString(res));
}

bool RegistryKey::IsExists(HKEY Key, const std::wstring &SubKey)
{
    HKEY key = nullptr;
    LSTATUS res = RegOpenKeyExW(Key,
                                SubKey.c_str(),
                                0,
                                KEY_READ,
                                &key);
    if(res == ERROR_SUCCESS)
    {
        RegCloseKey(key);
        return true;
    }
    else if(res == ERROR_FILE_NOT_FOUND || ERROR_PATH_NOT_FOUND)
        return false;
    else
        throw RegistryOperationException(L"cant check registry " + SubKey + L" key existance:" + Utils::GetErrorMessageString(res));
}

}