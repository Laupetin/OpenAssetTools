#pragma once

#include "Dumping/IZoneAssetDumperState.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

class AbstractMaterialConstantZoneState : public IZoneAssetDumperState
{
public:
    void EnsureInitialized();
    bool GetConstantName(unsigned hash, std::string& constantName) const;
    bool GetTextureDefName(unsigned hash, std::string& textureDefName) const;

    [[nodiscard]] virtual unsigned HashString(const std::string& str) const = 0;

protected:
    AbstractMaterialConstantZoneState();

    virtual void ExtractNamesFromShader(const void* shader, size_t shaderSize) = 0;
    virtual void ExtractNamesFromZoneInternal() = 0;
    virtual void AddStaticKnownNames() = 0;

    bool ShouldDumpFromStruct(const void* pStruct);
    void AddConstantName(const std::string& constantName);
    bool AddTextureDefName(const std::string& textureDefName);

    bool m_initialized;
    std::unordered_set<const void*> m_dumped_structs;
    std::unordered_map<unsigned, std::string> m_constant_names_from_shaders;
    std::unordered_map<unsigned, std::string> m_texture_def_names_from_shaders;
};

class AbstractMaterialConstantZoneStateDx9 : public AbstractMaterialConstantZoneState
{
protected:
    void ExtractNamesFromShader(const void* shader, size_t shaderSize) override;
};

class AbstractMaterialConstantZoneStateDx11 : public AbstractMaterialConstantZoneState
{
protected:
    void ExtractNamesFromShader(const void* shader, size_t shaderSize) override;
};
