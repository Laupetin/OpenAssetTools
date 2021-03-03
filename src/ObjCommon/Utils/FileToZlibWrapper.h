#pragma once

#include <ioapi.h>
#include <iostream>
#include <istream>
#include <ostream>

#include "ObjStream.h"

class FileToZlibWrapper
{
public:
    static zlib_filefunc_def CreateFunctions32ForFile(std::iostream* stream);
    static zlib_filefunc_def CreateFunctions32ForFile(std::istream* stream);
    static zlib_filefunc_def CreateFunctions32ForFile(std::ostream* stream);
};
