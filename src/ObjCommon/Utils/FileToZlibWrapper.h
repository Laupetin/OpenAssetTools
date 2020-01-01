#pragma once

#include <ioapi.h>
#include "Utils/FileAPI.h"

class FileToZlibWrapper
{
public:
    static zlib_filefunc_def CreateFunctions32ForFile(FileAPI::IFile* file);
};
