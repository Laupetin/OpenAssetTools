#include "Game/T6/T6.h"
#include "LoaderTechniqueSetT6.h"

#include <cstring>
#include <format>
#include <nlohmann/json.hpp>

using namespace T6;

namespace
{
    class TechniqueSetLoader final : public AssetCreator<AssetTechniqueSet>
    {
    public:
        TechniqueSetLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = std::format("techniquesets/{}.json", assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            MaterialTechniqueSet* techniqueSet = new MaterialTechniqueSet;
            memset(techniqueSet, 0, sizeof(MaterialTechniqueSet));

            const auto js = nlohmann::json::parse(*file.m_stream.get());

            std::string techSetName = js["name"];
            techniqueSet->name = _strdup(techSetName.c_str());
            techniqueSet->worldVertFormat = (unsigned char)js["worldVertFormat"];

            for (int i = 0; i < 36; i++)
            {
                nlohmann::json techniqueJs = js["techniques"][i];

                if (techniqueJs.size() == 0)
                {
                    techniqueSet->techniques[i] = NULL;
                }
                else
                {
                    MaterialTechnique* technique = new MaterialTechnique;
                    techniqueSet->techniques[i] = technique;

                    std::string techName = techniqueJs["name"];
                    technique->name = _strdup(techName.c_str());
                    technique->flags = techniqueJs["flags"];
                    technique->passCount = techniqueJs["passCount"];
                    _ASSERT(technique->passCount == 1);
                    
                    for (auto passIndex = 0u; passIndex < technique->passCount; passIndex++)
                    {
                        MaterialPass* currPass = &technique->passArray[passIndex];
                        nlohmann::json passJs = techniqueJs["passArray"][passIndex];

                        currPass->perPrimArgCount = (unsigned char)passJs["perPrimArgCount"];
                        currPass->perObjArgCount = (unsigned char)passJs["perObjArgCount"];
                        currPass->stableArgCount = (unsigned char)passJs["stableArgCount"];
                        currPass->customSamplerFlags = (unsigned char)passJs["customSamplerFlags"];
                        currPass->precompiledIndex = (unsigned char)passJs["precompiledIndex"];
                        currPass->materialType = (unsigned char)passJs["materialType"];

                        int argCount = currPass->perPrimArgCount + currPass->perObjArgCount + currPass->stableArgCount;
                        if (argCount == 0)
                        {
                            currPass->args = NULL;
                        }
                        else
                        {
                            currPass->args = new MaterialShaderArgument[argCount];
                            for (int i = 0; i < argCount; i++)
                            {
                                MaterialShaderArgument* currArg = &currPass->args[i];
                                auto currArgJs = passJs["args"][i];

                                currArg->type = currArgJs["type"];
                                currArg->location.offset = currArgJs["location"];
                                currArg->size = currArgJs["size"];
                                currArg->buffer = currArgJs["buffer"];
                                if (currArg->type == MTL_ARG_LITERAL_VERTEX_CONST || currArg->type == MTL_ARG_LITERAL_PIXEL_CONST)
                                {
                                    float* consts = new float[4];
                                    consts[0] = currArgJs["u"]["const0"];
                                    consts[1] = currArgJs["u"]["const1"];
                                    consts[2] = currArgJs["u"]["const2"];
                                    consts[3] = currArgJs["u"]["const3"];
                                    currArg->u.literalConst = (float(*)[4])consts;
                                }
                                else
                                {
                                    currArg->u.nameHash = currArgJs["u"]["value"];
                                }
                            }
                        }

                        if (passJs["vertexDecl"].size() == 0)
                        {
                            currPass->vertexDecl = NULL;
                        }
                        else
                        {
                            currPass->vertexDecl = new MaterialVertexDeclaration;
                            currPass->vertexDecl->streamCount = (unsigned char)passJs["vertexDecl"]["streamCount"];
                            currPass->vertexDecl->hasOptionalSource = passJs["vertexDecl"]["hasOptionalSource"];
                            currPass->vertexDecl->isLoaded = passJs["vertexDecl"]["isLoaded"];
                            for (int i = 0; i < 16; i++)
                            {
                                currPass->vertexDecl->routing.data[i].source = (unsigned char)passJs["vertexDecl"]["routing"][i]["source"];
                                currPass->vertexDecl->routing.data[i].dest = (unsigned char)passJs["vertexDecl"]["routing"][i]["dest"];
                            }
                        }

                        if (passJs["pixelShader"].size() == 0)
                        {
                            currPass->pixelShader = NULL;
                        }
                        else
                        {
                            currPass->pixelShader = new MaterialPixelShader;

                            std::string pixelName = passJs["pixelShader"]["name"];
                            currPass->pixelShader->name = _strdup(pixelName.c_str());
                            currPass->pixelShader->prog.ps = NULL;

                            const auto psFileName = std::format("techniquesets/shader_bin/ps_{}", pixelName);
                            const auto psFile = m_search_path.Open(psFileName);
                            if (!psFile.IsOpen())
                            {
                                printf("ERROR: Cant find pixel shader %s\n", psFileName.c_str());
                                return AssetCreationResult::Failure();
                            }
                            currPass->pixelShader->prog.loadDef.programSize = psFile.m_length;
                            currPass->pixelShader->prog.loadDef.program = new char[psFile.m_length];
                            psFile.m_stream->read(currPass->pixelShader->prog.loadDef.program, psFile.m_length);
                        }

                        if (passJs["vertexShader"].size() == 0)
                        {
                            currPass->vertexShader = NULL;
                        }
                        else
                        {
                            currPass->vertexShader = new MaterialVertexShader;

                            std::string vertexName = passJs["vertexShader"]["name"];
                            currPass->vertexShader->name = _strdup(vertexName.c_str());
                            currPass->vertexShader->prog.vs = NULL;

                            const auto vsFileName = std::format("techniquesets/shader_bin/vs_{}", vertexName);
                            const auto vsFile = m_search_path.Open(vsFileName);
                            if (!vsFile.IsOpen())
                            {
                                printf("ERROR: Cant find vertex shader %s\n", vsFileName.c_str());
                                return AssetCreationResult::Failure();
                            }
                            currPass->vertexShader->prog.loadDef.programSize = vsFile.m_length;
                            currPass->vertexShader->prog.loadDef.program = new char[vsFile.m_length];
                            vsFile.m_stream->read(currPass->vertexShader->prog.loadDef.program, vsFile.m_length);
                        }
                    }
                }
            }
            return AssetCreationResult::Success(context.AddAsset<AssetTechniqueSet>(assetName, techniqueSet));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetTechniqueSet>> CreateTechniqueSetLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<TechniqueSetLoader>(memory, searchPath);
    }
} // namespace T6
