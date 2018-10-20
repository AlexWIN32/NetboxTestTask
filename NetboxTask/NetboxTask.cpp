/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <tchar.h>
#include <windows.h>
#include <string>
#include <iostream>
#include "Deploy.h"
#include "ErrorUtils.h"
#include "Misc.h"
#include "MainProcessing.h"
#include "XGetopt.h"

DECLARE_EXCEPTION(ShellExecuteException)

static void RunAsAdmin(int argc, _TCHAR* argv[])
{
    std::wstring paramsStr;

    for(int p = 1; p < argc; p++)
        paramsStr += std::wstring(argv[p]) + ((p < argc - 1) ? L" " : L"");

    // Launch itself as administrator.
    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = argv[0];
    sei.lpParameters = paramsStr.c_str();
    sei.nShow = SW_SHOW;
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hInstApp = nullptr;

    BOOL ret = ShellExecuteExW(&sei);
    if(!ret)
    {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_CANCELLED)
            throw ShellExecuteException(L"ShellExecute() error:" + Utils::GetErrorMessageString());
        else
            exit(0);
    }
    else
        WaitForSingleObject(sei.hProcess, INFINITE);
}

static void PrintUsage(_TCHAR *ProgramName)
{
    std::wcout << "Usage:" << ProgramName << "[-i] [-u] [-p install path]" <<  std::endl
               << "-i install application" << std::endl
               << "-u uninstall application" << std::endl
               << "-p install path " << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
    if(Utils::IsRunAsAdmin() == FALSE)
        try
        {
            RunAsAdmin(argc, argv);
        }
        catch(const Exception &ex)
        {
            std::wcout << L"error occured: " << ex.What() << std::endl;
            return -1;
        }

    std::wstring installPath = L"C:\\Users\\" + Utils::GetUserNameString() + L"\\AppData\\Local\\TestFolder";

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

    if(install && uninstall)
    {
        PrintUsage(argv[0]);
        return 0;
    }

    try
    {
        if(install)
            Deploy::Install(installPath);
        else if(uninstall)
            Deploy::Uninstall();
        else
            MainProcessing();
    }
    catch(const Exception &ex)
    {
        std::wcout << L"error occured: " << ex.What() << std::endl;
        return -1;
    }

    return 0;
}

