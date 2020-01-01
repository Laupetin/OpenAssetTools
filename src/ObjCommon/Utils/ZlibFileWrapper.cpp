#include "ZlibFileWrapper.h"
#include <cassert>

voidpf Wrapper_Zlib_FileOpen(voidpf opaque, const char*, int)
{
    return opaque;
}

uLong Wrapper_Zlib_FileRead(voidpf, voidpf stream, void* buf, const uLong size)
{
    auto* file = reinterpret_cast<FileAPI::IFile*>(stream);

    return file->Read(buf, 1, size);
}

uLong Wrapper_Zlib_FileWrite(voidpf opaque, voidpf stream, const void* buf, const uLong size)
{
    auto* file = reinterpret_cast<FileAPI::IFile*>(stream);

    return file->Write(buf, 1, size);
}

long Wrapper_Zlib_FileTell(voidpf opaque, voidpf stream)
{
    auto* file = reinterpret_cast<FileAPI::IFile*>(stream);

    return static_cast<long>(file->Pos());
}

long Wrapper_Zlib_FileSeek(voidpf opaque, voidpf stream, const uLong offset, const int origin)
{
    auto* file = reinterpret_cast<FileAPI::IFile*>(stream);

    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR:
        file->Skip(offset);
        break;

    case ZLIB_FILEFUNC_SEEK_END:
        assert(offset == 0);
        file->GotoEnd();
        break;

    case ZLIB_FILEFUNC_SEEK_SET:
        file->Goto(offset);
        break;

    default:
        return -1;
    }

    return 0;
}

int Wrapper_Zlib_FileClose(voidpf opaque, voidpf stream)
{
    auto* file = reinterpret_cast<FileAPI::IFile*>(stream);

    if (file->IsOpen())
    {
        file->Close();
        return 0;
    }

    return -1;
}

int Wrapper_Zlib_FileError(voidpf opaque, voidpf stream)
{
    return 0;
}

zlib_filefunc_def ZlibFileWrapper::CreateFunctions32ForFile(FileAPI::IFile* file)
{
    return zlib_filefunc_def_s
    {
        Wrapper_Zlib_FileOpen,
        Wrapper_Zlib_FileRead,
        Wrapper_Zlib_FileWrite,
        Wrapper_Zlib_FileTell,
        Wrapper_Zlib_FileSeek,
        Wrapper_Zlib_FileClose,
        Wrapper_Zlib_FileError,
        file
    };
}
