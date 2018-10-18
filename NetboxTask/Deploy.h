/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include "Exception.h"
#include <string>

namespace Deploy
{

DECLARE_EXCEPTION(AlreadyInstalledException);
DECLARE_EXCEPTION(RegistryOperationException);

void Install(const std::wstring &InstallPath);
void Uninstall();

};