/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <string>
#include <windows.h>
#include "Exception.h"
#include "ErrorUtils.h"

namespace Utils
{

class Win32SerialisingStrategy
{
public:
    static HANDLE Open(const std::wstring &Path, const std::wstring &Mode)
    {
        UINT mode = 0;
        DWORD acces = 0;

        if(Mode == L"r")
        {
            mode = OPEN_EXISTING;
            acces = GENERIC_READ;
        }
        else if(Mode == L"w")
        {
            mode = OPEN_EXISTING;
            acces = GENERIC_WRITE;
        }
        else if(Mode == L"w+")
        {
            mode = CREATE_ALWAYS;
            acces = GENERIC_WRITE;
        }
        else 
            throw IOException(L"cant open " + Path + L": unknown mode " + Mode);

        HANDLE res = CreateFileW(Path.c_str(),
                                 acces,
                                 0,
                                 nullptr,
                                 mode,
                                 FILE_ATTRIBUTE_NORMAL,
                                 nullptr);

        if(res == INVALID_HANDLE_VALUE)
            throw IOException(L"cant open " + Path + L":" + Utils::GetErrorMessageString());

        return res;
    }
    template<class TVar>
    static size_t Read(HANDLE File, TVar *Array, size_t ElemsCount, const std::wstring &Path)
    {
        DWORD readedBytes;

        BOOL res = ReadFile(File,
                            Array,
                            sizeof(TVar) * ElemsCount,
                            &readedBytes,
                            nullptr);

        if(!res)
            throw IOException(L"cant read from " + Path + L":" + Utils::GetErrorMessageString());

        return static_cast<size_t>(readedBytes / sizeof(TVar));
    }
    template<class TVar>
    static void Write(HANDLE File, const TVar *Array, size_t ElemsCount, const std::wstring &Path)
    {
        DWORD writedBytes;

        BOOL res = WriteFile(File,
                             Array,
                             sizeof(TVar) * ElemsCount,
                             &writedBytes,
                             nullptr);

        size_t writedElems = static_cast<size_t>(writedBytes / sizeof(TVar));

        if(!res || writedElems != ElemsCount)
            throw IOException(L"cant write to " + Path + L":" + Utils::GetErrorMessageString());
    }
    static void Close(HANDLE File)
    {
        CloseHandle(File);
    }
};

template<class TStrategy, class TDescriptor>
class BasicFileGuard final
{
private:
    TDescriptor file = TDescriptor();
    std::wstring path;
public:
    BasicFileGuard(const BasicFileGuard &) = delete;
    BasicFileGuard &operator= (const BasicFileGuard &) = delete;

    BasicFileGuard(TDescriptor File, const std::wstring &Path)
        : file(File), path(Path)
    {}
    BasicFileGuard(const std::wstring &Path, const std::wstring &Mode)
        : path(Path)
    {
        file = TStrategy::Open(Path, Mode);
    }
    TDescriptor Get() const
    {
        return file;
    }
    ~BasicFileGuard()
    {
        TStrategy::Close(file);
    }
    template<class TVar>
    size_t Read(TVar *OutArray, size_t ElemsCount)
    {
        return TStrategy::Read<TVar>(file, OutArray, ElemsCount, path);
    }
    template<class TVar>
    void Write(const TVar *Array, size_t ArraySize)
    {
        TStrategy::Write<TVar>(file, Array, ArraySize, path);
    }
};

typedef BasicFileGuard<Win32SerialisingStrategy, HANDLE> FileGuard;

}