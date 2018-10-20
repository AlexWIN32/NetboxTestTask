/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <windows.h>
#include "MainProcessing.h"
#include "ErrorUtils.h"

static void CALLBACK TimerRoutine(PVOID, BOOLEAN)
{
    _exit(0);
}

void MainProcessing()
{
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

    MessageBoxW(nullptr, L"Programm wil automatically end in 30 seconds", L"Message", MB_OK);
}