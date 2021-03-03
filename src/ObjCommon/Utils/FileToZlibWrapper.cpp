#include "FileToZlibWrapper.h"

#include <cassert>
#include <fstream>

voidpf Wrapper_Zlib_FileOpen(voidpf opaque, const char*, int)
{
    return opaque;
}

template<typename T>
uLong Wrapper_Zlib_FileRead(voidpf opaque, voidpf stream, void* buf, const uLong size)
{
    auto* file = static_cast<T*>(stream);

    file->read(static_cast<char*>(buf), size);

    return static_cast<uLong>(file->gcount());
}

uLong Wrapper_Zlib_NoFileRead(voidpf opaque, voidpf stream, void* buf, const uLong size)
{
    return 0;
}

template<typename T>
uLong Wrapper_Zlib_FileWrite(voidpf opaque, voidpf stream, const void* buf, const uLong size)
{
    auto* file = static_cast<T*>(stream);
    file->write(static_cast<const char*>(buf), size);
    return size;
}

uLong Wrapper_Zlib_NoFileWrite(voidpf opaque, voidpf stream, const void* buf, const uLong size)
{
    return 0;
}

template<typename T>
long Wrapper_Zlib_FileTellRead(voidpf opaque, voidpf stream)
{
    auto* file = static_cast<T*>(stream);
    
    return static_cast<long>(file->tellg());
}

template<typename T>
long Wrapper_Zlib_FileTellWrite(voidpf opaque, voidpf stream)
{
    auto* file = static_cast<T*>(stream);
    
    return static_cast<long>(file->tellp());
}

template<typename T>
long Wrapper_Zlib_FileSeekRead(voidpf opaque, voidpf stream, const uLong offset, const int origin)
{
    auto* file = static_cast<T*>(stream);

    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR:
        file->seekg(offset, SEEK_CUR);
        break;

    case ZLIB_FILEFUNC_SEEK_END:
        file->seekg(offset, SEEK_END);
        break;

    case ZLIB_FILEFUNC_SEEK_SET:
        file->seekg(offset, SEEK_SET);
        break;

    default:
        return -1;
    }

    return 0;
}

template<typename T>
long Wrapper_Zlib_FileSeekWrite(voidpf opaque, voidpf stream, const uLong offset, const int origin)
{
    auto* file = static_cast<T*>(stream);

    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR:
        file->seekp(offset, SEEK_CUR);
        break;

    case ZLIB_FILEFUNC_SEEK_END:
        file->seekp(offset, SEEK_END);
        break;

    case ZLIB_FILEFUNC_SEEK_SET:
        file->seekp(offset, SEEK_SET);
        break;

    default:
        return -1;
    }

    return 0;
}

template<typename T>
int Wrapper_Zlib_FileClose(voidpf opaque, voidpf stream)
{
    return 0;
    // auto* file = static_cast<T*>(stream);
    //
    // if (file->is_open())
    // {
    //     file->close();
    //     return 0;
    // }
    //
    // return -1;
}

int Wrapper_Zlib_FileError(voidpf opaque, voidpf stream)
{
    return 0;
}

zlib_filefunc_def FileToZlibWrapper::CreateFunctions32ForFile(std::iostream* stream)
{
    return zlib_filefunc_def_s
    {
        Wrapper_Zlib_FileOpen,
        Wrapper_Zlib_FileRead<std::iostream>,
        Wrapper_Zlib_FileWrite<std::iostream>,
        Wrapper_Zlib_FileTellRead<std::iostream>,
        Wrapper_Zlib_FileSeekRead<std::iostream>,
        Wrapper_Zlib_FileClose<std::iostream>,
        Wrapper_Zlib_FileError,
        stream
    };
}

zlib_filefunc_def FileToZlibWrapper::CreateFunctions32ForFile(std::istream* stream)
{
    return zlib_filefunc_def_s
    {
        Wrapper_Zlib_FileOpen,
        Wrapper_Zlib_FileRead<std::istream>,
        Wrapper_Zlib_NoFileWrite,
        Wrapper_Zlib_FileTellRead<std::istream>,
        Wrapper_Zlib_FileSeekRead<std::istream>,
        Wrapper_Zlib_FileClose<std::istream>,
        Wrapper_Zlib_FileError,
        stream
    };
}

zlib_filefunc_def FileToZlibWrapper::CreateFunctions32ForFile(std::ostream* stream)
{
    return zlib_filefunc_def_s
    {
        Wrapper_Zlib_FileOpen,
        Wrapper_Zlib_NoFileRead,
        Wrapper_Zlib_FileWrite<std::ostream>,
        Wrapper_Zlib_FileTellWrite<std::ostream>,
        Wrapper_Zlib_FileSeekWrite<std::ostream>,
        Wrapper_Zlib_FileClose<std::ostream>,
        Wrapper_Zlib_FileError,
        stream
    };
}
