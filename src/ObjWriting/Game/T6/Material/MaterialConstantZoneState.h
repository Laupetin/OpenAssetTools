#pragma once

#include "Dumping/IZoneAssetDumperState.h"
#include "Game/T6/T6.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace T6
{
    class MaterialConstantZoneState final : public IZoneAssetDumperState
    {
    public:
        void ExtractNamesFromZone();
        bool GetConstantName(unsigned hash, std::string& constantName) const;
        bool GetTextureDefName(unsigned hash, std::string& textureDefName) const;

    private:
        void ExtractNamesFromTechnique(const MaterialTechnique* technique);
        void ExtractNamesFromShader(const char* shader, size_t shaderSize);
        void AddStaticKnownNames();
        void AddConstantName(std::string constantName);
        bool AddTextureDefName(std::string textureDefName);

        std::unordered_set<const MaterialTechnique*> m_dumped_techniques;
        std::unordered_map<unsigned, std::string> m_constant_names_from_shaders;
        std::unordered_map<unsigned, std::string> m_texture_def_names_from_shaders;
    };
} // namespace T6
