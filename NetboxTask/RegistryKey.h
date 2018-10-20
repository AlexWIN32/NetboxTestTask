/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <windows.h>
#include <string.h>
#include "Exception.h"

namespace Utils
{

DECLARE_EXCEPTION(RegistryOperationException);

class RegistryKey
{
public:
    enum Operation
    {
        MODE_OPEN,
        MODE_CREATE
    };
private:
    HKEY key = nullptr;
public:
    RegistryKey(HKEY Key, const std::wstring &SubKey, Operation Op);
    RegistryKey(const RegistryKey &) = delete;
    ~RegistryKey();
    HKEY Get() const { return key; }
    RegistryKey &operator= (const RegistryKey &) = delete;
    std::wstring ReadString(const std::wstring &VarName) const;
    void WriteString(const std::wstring &VarName, const std::wstring &VarValue);
    static void Delete(HKEY Key, const std::wstring &SubKey);
    static bool IsExists(HKEY Key, const std::wstring &SubKey);
};

}