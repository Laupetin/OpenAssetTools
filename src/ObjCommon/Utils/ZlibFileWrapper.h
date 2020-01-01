#pragma once

#include <ioapi.h>
#include "Utils/FileAPI.h"

class ZlibFileWrapper
{
public:
    static zlib_filefunc_def CreateFunctions32ForFile(FileAPI::IFile* file);
};
