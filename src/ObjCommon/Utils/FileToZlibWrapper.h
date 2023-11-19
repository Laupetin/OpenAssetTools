#pragma once

#include "ObjStream.h"

#include <ioapi.h>
#include <iostream>
#include <istream>
#include <ostream>

class FileToZlibWrapper
{
public:
    static zlib_filefunc_def CreateFunctions32ForFile(std::iostream* stream);
    static zlib_filefunc_def CreateFunctions32ForFile(std::istream* stream);
    static zlib_filefunc_def CreateFunctions32ForFile(std::ostream* stream);
};
