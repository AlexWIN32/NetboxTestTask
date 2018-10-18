/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/
/*
http://www.cplusplus.com/forum/windows/220942/
https://stackoverflow.com/questions/3079684/registering-windows-programs-to-installed-programs-list
https://docs.microsoft.com/en-us/windows/desktop/sysinfo/32-bit-and-64-bit-application-data-in-the-registry
https://docs.microsoft.com/en-us/windows/desktop/WinProg64/accessing-an-alternate-registry-view
https://docs.microsoft.com/en-us/windows/desktop/api/winreg/nf-winreg-regsavekeyexw
https://docs.microsoft.com/en-us/windows/desktop/api/winreg/nf-winreg-regcreatekeyexa
https://docs.microsoft.com/en-us/windows/desktop/SysInfo/registry-key-security-and-access-rights
https://stackoverflow.com/questions/29115493/how-to-use-regcreatekeyex
*/

#include <tchar.h>
#include <Windows.h>
#include <string>
#include <lmcons.h>
#include "Deploy.h"

static std::wstring GetUserNameString()
{
    WCHAR username[UNLEN + 1] = {};
    DWORD size = UNLEN + 1;

    GetUserNameW(reinterpret_cast<LPWSTR>(&username), &size);

    return username;
}

int _tmain(int argc, _TCHAR* argv[])
{
    std::wstring installPath = L"C:\\Users\\" + GetUserNameString() + L"\\AppData\\Local\\TestFolder";

    Deploy::Install(installPath);
    /*
    HKEY hKey;

    LSTATUS res = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                                 L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask",
                                 0,
                                 nullptr,
                                 REG_OPTION_NON_VOLATILE,
                                 KEY_WRITE | KEY_WOW64_32KEY,
                                 nullptr,
                                 &hKey,
                                 nullptr);


    if (res != ERROR_SUCCESS)
    {
        LPTSTR errorText = NULL;

        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                   res,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR)&errorText,
                   0,
                   NULL);

        if ( NULL != errorText )
        {
           // ... do something with the string `errorText` - log it, display it to the user, etc.

           // release memory allocated by FormatMessage()
           LocalFree(errorText);
           errorText = NULL;
        }
    }
    */

    return 0;
}

