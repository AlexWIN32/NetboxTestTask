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
/*
static BOOL IsElevated()
{
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;

    if( OpenProcessToken( GetCurrentProcess( ),TOKEN_QUERY,&hToken ) ) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof( TOKEN_ELEVATION );
        if( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize ) ) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if( hToken ) {
        CloseHandle( hToken );
    }
    return fRet;
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
*/

int _tmain(int argc, _TCHAR* argv[])
{
    /*
    if(!IsRunAsAdmin())
    {
        wchar_t szPath[MAX_PATH];
        if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
        {
            // Launch itself as administrator.
            SHELLEXECUTEINFO sei = { sizeof(sei) };
            sei.lpVerb = L"runas";
            sei.lpFile = szPath;
            sei.nShow = SW_SHOW;
            sei.fMask = SEE_MASK_NOCLOSEPROCESS;
            sei.hInstApp = NULL;

            bool ret = ShellExecuteExW(&sei);
            if (!ret)
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

    if(!IsElevated())
    {
        CREDUI_INFO cui;
        TCHAR pszName[CREDUI_MAX_USERNAME_LENGTH+1];
        TCHAR pszPwd[CREDUI_MAX_PASSWORD_LENGTH+1];
        BOOL fSave;
        DWORD dwErr;

        cui.cbSize = sizeof(CREDUI_INFO);
        cui.hwndParent = NULL;
        cui.pszMessageText = TEXT("Enter administrator account information");
        cui.pszCaptionText = TEXT("CredUITest");
        cui.hbmBanner = NULL;
        fSave = FALSE;
        SecureZeroMemory(pszName, sizeof(pszName));
        SecureZeroMemory(pszPwd, sizeof(pszPwd));
        dwErr = CredUIPromptForCredentialsW(
            &cui,                         // CREDUI_INFO structure
            TEXT("TheServer"),            // Target for credentials
                                          //   (usually a server)
            NULL,                         // Reserved
            0,                            // Reason
            pszName,                      // User name
            CREDUI_MAX_USERNAME_LENGTH+1, // Max number of char for user name
            pszPwd,                       // Password
            CREDUI_MAX_PASSWORD_LENGTH+1, // Max number of char for password
            &fSave,                       // State of save check box
            CREDUI_FLAGS_GENERIC_CREDENTIALS | CREDUI_FLAGS_ALWAYS_SHOW_UI | CREDUI_FLAGS_DO_NOT_PERSIST | CREDUI_FLAGS_EXPECT_CONFIRMATION );

        if(!dwErr)
        {

            DWORD ret = CredUIConfirmCredentialsW(TEXT("TheServer"), true);
            if(ret != NO_ERROR)
            {
                std::wstring errStr = Utils::GetErrorMessageString(ret);
                int asd = 0;
            }

            SecureZeroMemory(pszName, sizeof(pszName));
            SecureZeroMemory(pszPwd, sizeof(pszPwd));
        }
    }
    */
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

