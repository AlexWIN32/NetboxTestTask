/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <tchar.h>
#include <windows.h>
#include <wincred.h>
#include <string>
#include <lmcons.h>
#include <iostream>
#include "Deploy.h"
#include "ErrorUtils.h"
#include "XGetopt.h"

static std::wstring GetUserNameString()
{
    WCHAR username[UNLEN + 1] = {};
    DWORD size = UNLEN + 1;

    GetUserNameW(reinterpret_cast<LPWSTR>(&username), &size);

    return username;
}

BOOL IsRunAsAdmin()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in
    // the primary access token of the process.
    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsRunAsAdmin;
}


int _tmain(int argc, _TCHAR* argv[])
{
    if(!IsRunAsAdmin())
    {
        wchar_t szPath[MAX_PATH];
        if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
        {
            // Launch itself as administrator.
            SHELLEXECUTEINFO sei = { sizeof(sei) };
            sei.lpVerb = L"runas";
            sei.lpFile = szPath;
            sei.lpParameters = NULL;
            sei.nShow = SW_SHOW;
            sei.fMask = SEE_MASK_NOCLOSEPROCESS;
            sei.hInstApp = NULL;

            BOOL ret = ShellExecuteExW(&sei);
            if(!ret)
            {
                DWORD dwError = GetLastError();
                if (dwError == ERROR_CANCELLED)
                {
                    // The user refused the elevation.
                    // Do nothing ...
                }
            }
            else
                WaitForSingleObject(sei.hProcess,INFINITE);
        }
    }

    std::wstring installPath = L"C:\\Users\\" + GetUserNameString() + L"\\AppData\\Local\\TestFolder";

    bool install = false, uninstall = false;

    int c = 0;
    while((c = getopt(argc, argv, L"iup:")) != EOF)
    {
        if(c == 'i')
            install = true;
        else if(c == 'u')
            uninstall = true;
        else if(c == 'p')
        {
            if(optarg != nullptr)
                installPath = optarg;
        }
    }

    try
    {
        if(install)
            Deploy::Install(installPath);
        else if(uninstall)
            Deploy::Uninstall();
    }
    catch(const Exception &ex)
    {
        std::wcout << L"error occured: " << ex.What() << std::endl;
        return -1;
    }

    return 0;
}

