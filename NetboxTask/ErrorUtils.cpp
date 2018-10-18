/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include "ErrorUtils.h"

namespace Utils
{

std::wstring GetErrorMessageString(DWORD MessageId)
{
    LPWSTR errorText = nullptr;

    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr,
                  MessageId,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  reinterpret_cast<LPWSTR>(&errorText),
                  0,
                  nullptr);

    if(errorText == nullptr)
        return L"cant get error message";

    std::wstring str = errorText;

    LocalFree(errorText);
    
    return str;
}

std::wstring GetErrorMessageString()
{
    return GetErrorMessageString(GetLastError());
}

}
