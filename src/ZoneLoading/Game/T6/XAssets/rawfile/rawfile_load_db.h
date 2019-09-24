#pragma once
#include "Loading/AssetLoader.h"
#include "Game/T6/T6.h"

namespace T6
{
    class RawFileLoader final : public AssetLoader
    {
        void LoadRawFile(RawFile* pRawFile, bool atStreamStart);
        void LoadRawFileAsset(RawFile** pPtr);

    public:
        RawFileLoader(IZoneScriptStringProvider* scriptStringProvider, Zone* zone, IZoneInputStream* stream);

		void LoadRawFilePtr(RawFile** pPtr);
        static std::string GetRawFileName(RawFile* pRawFile);
    };
}
