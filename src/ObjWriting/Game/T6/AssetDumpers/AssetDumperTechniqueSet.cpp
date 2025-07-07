#include "AssetDumperTechniqueSet.h"

#include <nlohmann/json.hpp>
#include <sstream>
#include <unordered_set>

using namespace T6;

class ShaderZoneState final : public IZoneAssetDumperState
{
public:
    bool ShouldDumpTechnique(const MaterialTechnique* technique)
    {
        const auto existingTechnique = m_dumped_techniques.find(technique);
        if (existingTechnique == m_dumped_techniques.end())
        {
            m_dumped_techniques.emplace(technique);
            return true;
        }

        return false;
    }

    bool ShouldDumpPixelShader(const MaterialPixelShader* pixelShader)
    {
        const auto existingPixelShader = m_dumped_pixel_shaders.find(pixelShader);
        if (existingPixelShader == m_dumped_pixel_shaders.end())
        {
            m_dumped_pixel_shaders.emplace(pixelShader);
            return true;
        }

        return false;
    }

    bool ShouldDumpVertexShader(const MaterialVertexShader* vertexShader)
    {
        const auto existingVertexShader = m_dumped_vertex_shaders.find(vertexShader);
        if (existingVertexShader == m_dumped_vertex_shaders.end())
        {
            m_dumped_vertex_shaders.emplace(vertexShader);
            return true;
        }

        return false;
    }

private:
    std::unordered_set<const MaterialTechnique*> m_dumped_techniques;
    std::unordered_set<const MaterialPixelShader*> m_dumped_pixel_shaders;
    std::unordered_set<const MaterialVertexShader*> m_dumped_vertex_shaders;
};

bool AssetDumperTechniqueSet::ShouldDump(XAssetInfo<MaterialTechniqueSet>* asset)
{
    return true;
}

void AssetDumperTechniqueSet::DumpPixelShader(const AssetDumpingContext& context, const MaterialPixelShader* pixelShader)
{
    std::ostringstream ss;
    ss << "techniquesets/shader_bin/ps_" << pixelShader->name;

    const auto shaderFile = context.OpenAssetFile(ss.str());

    if (!shaderFile)
        return;

    shaderFile->write(pixelShader->prog.loadDef.program, pixelShader->prog.loadDef.programSize);
}

void AssetDumperTechniqueSet::DumpVertexShader(const AssetDumpingContext& context, const MaterialVertexShader* vertexShader)
{
    std::ostringstream ss;
    ss << "techniquesets/shader_bin/vs_" << vertexShader->name;

    const auto shaderFile = context.OpenAssetFile(ss.str());

    if (!shaderFile)
        return;

    shaderFile->write(vertexShader->prog.loadDef.program, vertexShader->prog.loadDef.programSize);
}

void AssetDumperTechniqueSet::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialTechniqueSet>* asset)
{
    const auto* techniqueSet = asset->Asset();
    auto* shaderState = context.GetZoneAssetDumperState<ShaderZoneState>();

    const auto assetFile = context.OpenAssetFile(std::format("techniquesets/{}.json", techniqueSet->name));
    if (!assetFile)
        return;

    nlohmann::json js;

    js["name"] = techniqueSet->name;
    js["worldVertFormat"] = techniqueSet->worldVertFormat;

    js["techniques"] = nlohmann::json::array();
    for (const auto* technique : techniqueSet->techniques)
    {
        nlohmann::json techniqueJs = nlohmann::json::object();

        if (technique != NULL)
        {
            techniqueJs["name"] = technique->name;
            techniqueJs["flags"] = technique->flags;
            techniqueJs["passCount"] = technique->passCount;

            _ASSERT(technique->passCount == 1);

            techniqueJs["passArray"] = nlohmann::json::array();
            for (auto passIndex = 0u; passIndex < technique->passCount; passIndex++)
            {
                const MaterialPass* currPass = &technique->passArray[passIndex];
                nlohmann::json passJs = nlohmann::json::object();

                passJs["perPrimArgCount"] = currPass->perPrimArgCount;
                passJs["perObjArgCount"] = currPass->perObjArgCount;
                passJs["stableArgCount"] = currPass->stableArgCount;
                passJs["customSamplerFlags"] = currPass->customSamplerFlags;
                passJs["precompiledIndex"] = currPass->precompiledIndex;
                passJs["materialType"] = currPass->materialType;

                nlohmann::json vertDeclJs = nlohmann::json::object();
                if (currPass->vertexDecl != NULL)
                {
                    vertDeclJs["streamCount"] = currPass->vertexDecl->streamCount;
                    vertDeclJs["hasOptionalSource"] = currPass->vertexDecl->hasOptionalSource;
                    vertDeclJs["isLoaded"] = currPass->vertexDecl->isLoaded;
                    for (int i = 0; i < 16; i++)
                    {
                        vertDeclJs["routing"][i]["source"] = currPass->vertexDecl->routing.data[i].source;
                        vertDeclJs["routing"][i]["dest"] = currPass->vertexDecl->routing.data[i].dest;

                        _ASSERT(currPass->vertexDecl->routing.decl[i] == NULL);
                    }
                }
                passJs["vertexDecl"] = vertDeclJs;

                passJs["args"] = nlohmann::json::array();
                if (currPass->args != NULL)
                {
                    for (int i = 0; i < currPass->perPrimArgCount + currPass->perObjArgCount + currPass->stableArgCount; i++)
                    {
                        nlohmann::json argsJs = nlohmann::json::object();
                        MaterialShaderArgument* currArg = &currPass->args[i];

                        argsJs["type"] = currArg->type;
                        argsJs["location"] = currArg->location.offset;
                        argsJs["size"] = currArg->size;
                        argsJs["buffer"] = currArg->buffer;
                        if (currArg->type == MTL_ARG_LITERAL_VERTEX_CONST || currArg->type == MTL_ARG_LITERAL_PIXEL_CONST)
                        {
                            argsJs["u"]["const0"] = currArg->u.literalConst[0];
                            argsJs["u"]["const1"] = currArg->u.literalConst[1];
                            argsJs["u"]["const2"] = currArg->u.literalConst[2];
                            argsJs["u"]["const3"] = currArg->u.literalConst[3];
                        }
                        else
                        {
                            argsJs["u"]["value"] = currArg->u.nameHash;
                        }

                        passJs["args"].push_back(argsJs);
                    }
                }

                nlohmann::json pixelJs = nlohmann::json::object();
                if (currPass->pixelShader != NULL)
                {
                    pixelJs["name"] = currPass->pixelShader->name;
                    if (shaderState->ShouldDumpPixelShader(currPass->pixelShader))
                        DumpPixelShader(context, currPass->pixelShader);
                }
                passJs["pixelShader"] = pixelJs;

                nlohmann::json vertexJs = nlohmann::json::object();
                if (currPass->vertexShader != NULL)
                {
                    vertexJs["name"] = currPass->vertexShader->name;
                    if (shaderState->ShouldDumpVertexShader(currPass->vertexShader))
                        DumpVertexShader(context, currPass->vertexShader);
                }
                passJs["vertexShader"] = vertexJs;

                techniqueJs["passArray"].push_back(passJs);
            }
        }

        js["techniques"].push_back(techniqueJs);
    }

    std::string jsonString = js.dump(4);
    assetFile->write(jsonString.c_str(), jsonString.size());
}
