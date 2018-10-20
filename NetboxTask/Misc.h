/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <windows.h>
#include <string>
#include <lmcons.h>
#include "Exception.h"

namespace Utils
{

DECLARE_EXCEPTION(AdministratorRightsCheckException)

inline void GetExeFileData(std::wstring &Path, std::wstring &Name)
{
    WCHAR path[MAX_PATH + 1] = {};

    GetModuleFileNameW(nullptr, path, MAX_PATH + 1);

    std::wstring pathStr = path;

    size_t lastSlashPos = pathStr.find_last_of('\\');

    Path = pathStr.substr(0, lastSlashPos);
    Name = pathStr.substr(lastSlashPos + 1);
}

inline std::wstring GetUserNameString()
{
    WCHAR username[UNLEN + 1] = {};
    DWORD size = UNLEN + 1;

    GetUserNameW(reinterpret_cast<LPWSTR>(&username), &size);

    return username;
}

inline BOOL IsRunAsAdmin()
{
    BOOL isRunAsAdmin = false;
    PSID administratorsGroup = nullptr;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    BOOL res = AllocateAndInitializeSid(&NtAuthority,
                                        2,
                                        SECURITY_BUILTIN_DOMAIN_RID,
                                        DOMAIN_ALIAS_RID_ADMINS,
                                        0, 0, 0, 0, 0, 0,
                                        &administratorsGroup);
    if (!res)
    {
        if(administratorsGroup != nullptr)
            FreeSid(administratorsGroup);

        throw AdministratorRightsCheckException(L"cant allocate and initialize sid:" + Utils::GetErrorMessageString(GetLastError()));
    }

    // Determine whether the SID of administrators group is enabled in
    // the primary access token of the process.
    if (!CheckTokenMembership(NULL, administratorsGroup, &isRunAsAdmin))
    {
        if(administratorsGroup != nullptr)
            FreeSid(administratorsGroup);

        throw AdministratorRightsCheckException(L"cant check token membership:" + Utils::GetErrorMessageString(GetLastError()));
    }

    return isRunAsAdmin;
}

}