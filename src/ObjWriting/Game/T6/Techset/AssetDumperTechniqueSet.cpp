#include "AssetDumperTechniqueSet.h"

#include <nlohmann/json.hpp>
#include <sstream>
#include <unordered_set>

using namespace T6;

void AssetDumperTechniqueSet::DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetTechniqueSet::Type>& asset)
{
    const auto* techniqueSet = asset.Asset();

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

        if (technique != nullptr)
        {
            techniqueJs["name"] = technique->name;
            techniqueJs["flags"] = technique->flags;
            techniqueJs["passCount"] = technique->passCount;

            assert(technique->passCount == 1);

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
                if (currPass->vertexDecl != nullptr)
                {
                    vertDeclJs["streamCount"] = currPass->vertexDecl->streamCount;
                    vertDeclJs["hasOptionalSource"] = currPass->vertexDecl->hasOptionalSource;
                    vertDeclJs["isLoaded"] = currPass->vertexDecl->isLoaded;
                    for (int i = 0; i < 16; i++)
                    {
                        vertDeclJs["routing"][i]["source"] = currPass->vertexDecl->routing.data[i].source;
                        vertDeclJs["routing"][i]["dest"] = currPass->vertexDecl->routing.data[i].dest;

                        assert(currPass->vertexDecl->routing.decl[i] == NULL);
                    }
                }
                passJs["vertexDecl"] = vertDeclJs;

                passJs["args"] = nlohmann::json::array();
                if (currPass->args != nullptr)
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
                if (currPass->pixelShader != nullptr)
                {
                    pixelJs["name"] = currPass->pixelShader->name;
                }
                passJs["pixelShader"] = pixelJs;

                nlohmann::json vertexJs = nlohmann::json::object();
                if (currPass->vertexShader != nullptr)
                {
                    vertexJs["name"] = currPass->vertexShader->name;
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
