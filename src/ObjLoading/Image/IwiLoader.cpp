#include "IwiLoader.h"

IwiLoader::IwiLoader(MemoryManager* memoryManager)
{
    m_memory_manager = memoryManager;
}

Texture* IwiLoader::LoadIwi(FileAPI::IFile* file)
{
    struct
    {
        char tag[3];
        char version;
    } iwiHeaderMeta{};

    if (file->Read(&iwiHeaderMeta, sizeof iwiHeaderMeta, 1) != 1)
        return nullptr;

    printf("Read IWI with version %i\n", iwiHeaderMeta.version);

    // TODO: Read iwi based on version

    return nullptr;
}
