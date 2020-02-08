#include "IwiLoader.h"

IwiLoader::IwiLoader(MemoryManager* memoryManager)
{
    m_memory_manager = memoryManager;
}

Texture* IwiLoader::LoadIwi(FileAPI::IFile* file)
{
    struct IWIHeaderMeta
    {
        char tag[3];
        char version;
    } iwiHeaderMeta{};

    file->GotoEnd();
    auto iwiSize = static_cast<size_t>(file->Pos());
    file->Goto(0);

    auto* buffer = new uint8_t[iwiSize];
    if(file->Read(buffer, 1, iwiSize) != iwiSize)
    {
        delete[] buffer;
        return nullptr;
    }

    auto* meta = reinterpret_cast<IWIHeaderMeta*>(buffer);

    printf("Read IWI with version %i\n", meta->version);

    // TODO: Read iwi based on version

    return nullptr;

    // if (file->Read(&iwiHeaderMeta, sizeof iwiHeaderMeta, 1) != 1)
    //     return nullptr;
    //
    // printf("Read IWI with version %i\n", iwiHeaderMeta.version);
    //
    // return nullptr;
}
