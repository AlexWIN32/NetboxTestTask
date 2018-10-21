/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <windows.h>
#include "MainProcessing.h"
#include "ErrorUtils.h"
#include "RegistryKey.h"
#include "Misc.h"

static void CALLBACK TimerRoutine(PVOID, BOOLEAN)
{
    _exit(0);
}

void MainProcessing()
{
    std::wstring installLocation;

    {
        Utils::RegistryKey key = {HKEY_LOCAL_MACHINE,
                                  L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\NetboxTask",
                                  Utils::RegistryKey::MODE_OPEN};

        installLocation = key.ReadString(L"InstallLocation");
    }

    std::wstring exePath, exeFile;
    Utils::GetExeFileData(exePath, exeFile);

    if(exePath != installLocation)
        throw InvalidExecutionPathException(L"program must be executed exactly from the installed path");

    HANDLE timerQueue = CreateTimerQueue();
    if(timerQueue == nullptr)
        throw MainProcessingException(L"cant create timer queue " + Utils::GetErrorMessageString());

    HANDLE timer;
    bool res = CreateTimerQueueTimer(&timer,
                                     timerQueue,
                                     static_cast<WAITORTIMERCALLBACK>(TimerRoutine),
                                     &timerQueue,
                                     30000,
                                     0,
                                     0);
    if(!res)
        throw MainProcessingException(L"cant create timer queue timer" + Utils::GetErrorMessageString());

    MessageBoxW(nullptr, L"Program will automatically end in 30 seconds", L"Message", MB_OK);
}