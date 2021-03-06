/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include "Exception.h"

DECLARE_EXCEPTION(MainProcessingException)
DECLARE_EXCEPTION(InvalidExecutionPathException)

void MainProcessing();