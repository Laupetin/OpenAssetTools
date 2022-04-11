#include "AssetLoaderTechniqueSet.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <type_traits>

#include "Utils/ClassUtils.h"
#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Pool/GlobalAssetPool.h"
#include "Techset/TechniqueFileReader.h"
#include "Techset/TechsetFileReader.h"
#include "Shader/D3D9ShaderAnalyser.h"

using namespace IW4;

namespace IW4
{
    class LoadedTechnique
    {
    public:
        MaterialTechnique* m_technique;
        std::vector<XAssetInfoGeneric*> m_dependencies;

        LoadedTechnique(MaterialTechnique* technique, std::vector<XAssetInfoGeneric*> dependencies)
            : m_technique(technique),
              m_dependencies(std::move(dependencies))
        {
        }
    };

    class TechniqueZoneLoadingState final : public IZoneAssetLoaderState
    {
    public:
        typedef const float (*literal_t)[4];

    private:
        std::unordered_map<std::string, std::unique_ptr<LoadedTechnique>> m_loaded_techniques;
        std::map<techset::ShaderArgumentLiteralSource, literal_t> m_allocated_literals;

    public:
        _NODISCARD const LoadedTechnique* FindLoadedTechnique(const std::string& techniqueName) const
        {
            const auto loadedTechnique = m_loaded_techniques.find(techniqueName);
            if (loadedTechnique != m_loaded_techniques.end())
                return loadedTechnique->second.get();

            return nullptr;
        }

        const LoadedTechnique* AddLoadedTechnique(std::string techniqueName, MaterialTechnique* technique, std::vector<XAssetInfoGeneric*> dependencies)
        {
            return m_loaded_techniques.emplace(std::make_pair(std::move(techniqueName), std::make_unique<LoadedTechnique>(technique, std::move(dependencies)))).first->second.get();
        }

        literal_t GetAllocatedLiteral(MemoryManager* memory, techset::ShaderArgumentLiteralSource source)
        {
            const auto& existingEntry = m_allocated_literals.find(source);

            if (existingEntry != m_allocated_literals.end())
                return existingEntry->second;

            auto* newLiteral = static_cast<float(*)[4]>(memory->Alloc(sizeof(float) * 4u));
            (*newLiteral)[0] = source.m_value[0];
            (*newLiteral)[1] = source.m_value[1];
            (*newLiteral)[2] = source.m_value[2];
            (*newLiteral)[3] = source.m_value[3];
            m_allocated_literals.emplace(std::make_pair(source, newLiteral));

            return newLiteral;
        }
    };

    class TechniqueCreator final : public techset::ITechniqueDefinitionAcceptor
    {
        MemoryManager* const m_memory;
        IAssetLoadingManager* const m_manager;
        TechniqueZoneLoadingState* const m_zone_state;

    public:
        struct Pass
        {
            XAssetInfo<MaterialVertexShader>* m_vertex_shader;
            std::unique_ptr<d3d9::ShaderInfo> m_vertex_shader_info;
            std::vector<bool> m_handled_vertex_shader_arguments;

            XAssetInfo<MaterialPixelShader>* m_pixel_shader;
            std::unique_ptr<d3d9::ShaderInfo> m_pixel_shader_info;
            std::vector<bool> m_handled_pixel_shader_arguments;

            MaterialVertexDeclaration m_vertex_decl;
            XAssetInfo<MaterialVertexDeclaration>* m_vertex_decl_asset;
            std::vector<MaterialShaderArgument> m_arguments;

            Pass()
                : m_vertex_shader(nullptr),
                  m_pixel_shader(nullptr),
                  m_vertex_decl{},
                  m_vertex_decl_asset(nullptr)
            {
            }
        };

        std::vector<Pass> m_passes;
        std::vector<XAssetInfoGeneric*> m_dependencies;

        TechniqueCreator(MemoryManager* memory, IAssetLoadingManager* manager, TechniqueZoneLoadingState* zoneState)
            : m_memory(memory),
              m_manager(manager),
              m_zone_state(zoneState)
        {
        }

        void AcceptNextPass() override
        {
            m_passes.emplace_back();
        }

        bool AutoCreateVertexShaderArguments(std::string& string)
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            for (auto i = 0u; i < pass.m_handled_vertex_shader_arguments.size(); i++)
            {
                if (!pass.m_handled_vertex_shader_arguments[i])
                {
                    std::cout << "Unhandled vertex shader \"" << pass.m_vertex_shader->m_name << "\" arg: " << pass.m_vertex_shader_info->m_constants[i].m_name << "\n";
                }
            }

            return true;
        }

        bool AutoCreatePixelShaderArguments(std::string& string)
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            for (auto i = 0u; i < pass.m_handled_pixel_shader_arguments.size(); i++)
            {
                if (!pass.m_handled_pixel_shader_arguments[i])
                {
                    std::cout << "Unhandled pixel shader \"" << pass.m_pixel_shader->m_name << "\" arg: " << pass.m_pixel_shader_info->m_constants[i].m_name << "\n";
                }
            }

            return true;
        }

        void AllocateVertexDecl()
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            std::sort(std::begin(pass.m_vertex_decl.routing.data), std::begin(pass.m_vertex_decl.routing.data) + pass.m_vertex_decl.streamCount,
                      [](const MaterialStreamRouting& r1, const MaterialStreamRouting& r2)
                      {
                          return r1.source < r2.source;
                      });

            std::ostringstream ss;
            for (auto i = 0u; i < pass.m_vertex_decl.streamCount; i++)
            {
                const auto& stream = pass.m_vertex_decl.routing.data[i];
                assert(stream.source < std::extent_v<decltype(materialStreamSourceAbbreviation)>);
                assert(stream.dest < std::extent_v<decltype(materialStreamDestinationAbbreviation)>);

                ss << materialStreamSourceAbbreviation[stream.source] << materialStreamDestinationAbbreviation[stream.dest];
            }

            pass.m_vertex_decl_asset = reinterpret_cast<XAssetInfo<MaterialVertexDeclaration>*>(m_manager->LoadDependency(ASSET_TYPE_VERTEXDECL, ss.str()));
        }

        bool AcceptEndPass(std::string& errorMessage) override
        {
            if (!AutoCreateVertexShaderArguments(errorMessage) || !AutoCreatePixelShaderArguments(errorMessage))
                return false;

            AllocateVertexDecl();

            return true;
        }

        void AcceptStateMap(const std::string& stateMapName) override
        {
            // TODO: State maps currently are not used
        }

        bool AcceptVertexShader(const std::string& vertexShaderName, std::string& errorMessage) override
        {
            auto* vertexShaderDependency = m_manager->LoadDependency(ASSET_TYPE_VERTEXSHADER, vertexShaderName);
            if (vertexShaderDependency == nullptr)
            {
                errorMessage = "Failed to load specified shader";
                return false;
            }

            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);
            pass.m_vertex_shader = reinterpret_cast<XAssetInfo<MaterialVertexShader>*>(vertexShaderDependency);

            const auto& shaderLoadDef = pass.m_vertex_shader->Asset()->prog.loadDef;
            pass.m_vertex_shader_info = d3d9::ShaderAnalyser::GetShaderInfo(shaderLoadDef.program, shaderLoadDef.programSize * sizeof(uint32_t));

            if (!pass.m_vertex_shader_info)
            {
                errorMessage = "No shader info for shader";
                return false;
            }

            pass.m_handled_vertex_shader_arguments.resize(pass.m_vertex_shader_info->m_constants.size());
            return true;
        }

        bool AcceptPixelShader(const std::string& pixelShaderName, std::string& errorMessage) override
        {
            auto* pixelShaderDependency = m_manager->LoadDependency(ASSET_TYPE_PIXELSHADER, pixelShaderName);
            if (pixelShaderDependency == nullptr)
            {
                errorMessage = "Failed to load specified shader";
                return false;
            }

            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);
            pass.m_pixel_shader = reinterpret_cast<XAssetInfo<MaterialPixelShader>*>(pixelShaderDependency);

            const auto& shaderLoadDef = pass.m_pixel_shader->Asset()->prog.loadDef;
            pass.m_pixel_shader_info = d3d9::ShaderAnalyser::GetShaderInfo(shaderLoadDef.program, shaderLoadDef.programSize * sizeof(uint32_t));

            if (!pass.m_pixel_shader_info)
            {
                errorMessage = "No shader info for shader";
                return false;
            }

            pass.m_handled_pixel_shader_arguments.resize(pass.m_pixel_shader_info->m_constants.size());
            return true;
        }

        static bool IsSamplerArgument(const d3d9::ShaderConstant& constant)
        {
            return constant.m_type == d3d9::ParameterType::SAMPLER
                || constant.m_type == d3d9::ParameterType::SAMPLER_1D
                || constant.m_type == d3d9::ParameterType::SAMPLER_2D
                || constant.m_type == d3d9::ParameterType::SAMPLER_3D
                || constant.m_type == d3d9::ParameterType::SAMPLER_CUBE;
        }

        static const CodeConstantSource* FindCodeConstantSource(const std::vector<std::string>& accessors, const CodeConstantSource* sourceTable)
        {
            const CodeConstantSource* foundSource = nullptr;
            const CodeConstantSource* currentTable = sourceTable;
            for (const auto& accessor : accessors)
            {
                if (currentTable == nullptr)
                    return nullptr;

                while (true)
                {
                    if (currentTable->name == nullptr)
                        return nullptr;

                    if (accessor == currentTable->name)
                        break;

                    currentTable++;
                }

                foundSource = currentTable;
                currentTable = currentTable->subtable;
            }

            return foundSource;
        }

        static const CodeSamplerSource* FindCodeSamplerSource(const std::vector<std::string>& accessors, const CodeSamplerSource* sourceTable)
        {
            const CodeSamplerSource* foundSource = nullptr;
            const CodeSamplerSource* currentTable = sourceTable;
            for (const auto& accessor : accessors)
            {
                if (currentTable == nullptr)
                    return nullptr;

                while (true)
                {
                    if (currentTable->name == nullptr)
                        return nullptr;

                    if (accessor == currentTable->name)
                        break;

                    currentTable++;
                }

                foundSource = currentTable;
                currentTable = currentTable->subtable;
            }

            return foundSource;
        }

        bool AcceptVertexShaderConstantArgument(techset::ShaderArgument shaderArgument, const techset::ShaderArgumentCodeSource& source, std::string& errorMessage)
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            if (!pass.m_vertex_shader_info)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            const auto& shaderInfo = *pass.m_vertex_shader_info;
            const auto matchingShaderConstant = std::find_if(shaderInfo.m_constants.begin(), shaderInfo.m_constants.end(), [&shaderArgument](const d3d9::ShaderConstant& constant)
            {
                return constant.m_name == shaderArgument.m_argument_name;
            });

            if (matchingShaderConstant == shaderInfo.m_constants.end())
            {
                errorMessage = "Could not find argument in shader";
                return false;
            }

            const auto shaderConstantIndex = static_cast<size_t>(matchingShaderConstant - shaderInfo.m_constants.begin());
            const auto argumentIsSampler = IsSamplerArgument(*matchingShaderConstant);
            if (argumentIsSampler)
            {
                errorMessage = "Vertex shader argument expected sampler but got constant";
                return false;
            }

            MaterialShaderArgument argument{};
            argument.type = MTL_ARG_CODE_VERTEX_CONST;
            argument.dest = static_cast<uint16_t>(matchingShaderConstant->m_register_index);

            const CodeConstantSource* constantSource = FindCodeConstantSource(source.m_accessors, s_codeConsts);
            if (!constantSource)
                constantSource = FindCodeConstantSource(source.m_accessors, s_defaultCodeConsts);

            if (!constantSource)
            {
                errorMessage = "Unknown code constant";
                return false;
            }

            if (constantSource->arrayCount > 0)
            {
                if (!source.m_index_accessor_specified)
                {
                    errorMessage = "Code constant must have array index specified";
                    return false;
                }

                if (source.m_index_accessor >= static_cast<size_t>(constantSource->arrayCount))
                {
                    errorMessage = "Code constant array index out of bounds";
                    return false;
                }

                argument.u.codeConst.index = static_cast<uint16_t>(constantSource->source + source.m_index_accessor);
            }
            else if (source.m_index_accessor_specified)
            {
                errorMessage = "Code constant cannot have array index specified";
                return false;
            }
            else
            {
                argument.u.codeConst.index = static_cast<uint16_t>(constantSource->source);
            }
            argument.u.codeConst.firstRow = 0u;
            argument.u.codeConst.rowCount = static_cast<unsigned char>(matchingShaderConstant->m_type_rows);

            pass.m_arguments.push_back(argument);
            pass.m_handled_vertex_shader_arguments[shaderConstantIndex] = true;

            return true;
        }

        bool AcceptPixelShaderCodeArgument(techset::ShaderArgument shaderArgument, const techset::ShaderArgumentCodeSource& source, std::string& errorMessage, const bool isSampler)
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            if (!pass.m_pixel_shader_info)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            const auto& shaderInfo = *pass.m_pixel_shader_info;

            const auto matchingShaderConstant = std::find_if(shaderInfo.m_constants.begin(), shaderInfo.m_constants.end(), [&shaderArgument](const d3d9::ShaderConstant& constant)
            {
                return constant.m_name == shaderArgument.m_argument_name;
            });

            if (matchingShaderConstant == shaderInfo.m_constants.end())
            {
                errorMessage = "Could not find argument in shader";
                return false;
            }

            const auto shaderConstantIndex = static_cast<size_t>(matchingShaderConstant - shaderInfo.m_constants.begin());
            const auto argumentIsSampler = IsSamplerArgument(*matchingShaderConstant);
            if (argumentIsSampler && !isSampler)
            {
                errorMessage = "Pixel shader argument expects sampler but got constant";
                return false;
            }
            else if (!argumentIsSampler && isSampler)
            {
                errorMessage = "Pixel shader argument expects constant but got sampler";
                return false;
            }

            MaterialShaderArgument argument{};
            argument.type = isSampler ? MTL_ARG_CODE_PIXEL_SAMPLER : MTL_ARG_CODE_PIXEL_CONST;
            argument.dest = static_cast<uint16_t>(matchingShaderConstant->m_register_index);

            unsigned sourceIndex, arrayCount;
            if (isSampler)
            {
                const CodeSamplerSource* samplerSource = FindCodeSamplerSource(source.m_accessors, s_codeSamplers);
                if (!samplerSource)
                    samplerSource = FindCodeSamplerSource(source.m_accessors, s_defaultCodeSamplers);

                if (!samplerSource)
                {
                    errorMessage = "Unknown code sampler";
                    return false;
                }

                sourceIndex = static_cast<unsigned>(samplerSource->source);
                arrayCount = static_cast<unsigned>(samplerSource->arrayCount);
            }
            else
            {
                const CodeConstantSource* constantSource = FindCodeConstantSource(source.m_accessors, s_codeConsts);
                if (!constantSource)
                    constantSource = FindCodeConstantSource(source.m_accessors, s_defaultCodeConsts);

                if (!constantSource)
                {
                    errorMessage = "Unknown code constant";
                    return false;
                }

                sourceIndex = static_cast<unsigned>(constantSource->source);
                arrayCount = static_cast<unsigned>(constantSource->arrayCount);
            }

            if (arrayCount > 0u)
            {
                if (!source.m_index_accessor_specified)
                {
                    errorMessage = "Code constant must have array index specified";
                    return false;
                }

                if (source.m_index_accessor >= arrayCount)
                {
                    errorMessage = "Code constant array index out of bounds";
                    return false;
                }

                argument.u.codeConst.index = static_cast<uint16_t>(sourceIndex + source.m_index_accessor);
            }
            else if (source.m_index_accessor_specified)
            {
                errorMessage = "Code constant cannot have array index specified";
                return false;
            }
            else
            {
                argument.u.codeConst.index = static_cast<uint16_t>(sourceIndex);
            }
            argument.u.codeConst.firstRow = 0u;
            argument.u.codeConst.rowCount = static_cast<unsigned char>(matchingShaderConstant->m_type_rows);

            pass.m_arguments.push_back(argument);
            pass.m_handled_pixel_shader_arguments[shaderConstantIndex] = true;

            return true;
        }

        bool AcceptShaderConstantArgument(const techset::ShaderSelector shader, const techset::ShaderArgument shaderArgument, const techset::ShaderArgumentCodeSource source,
                                          std::string& errorMessage) override
        {
            if (shader == techset::ShaderSelector::VERTEX_SHADER)
                return AcceptVertexShaderConstantArgument(shaderArgument, source, errorMessage);

            assert(shader == techset::ShaderSelector::PIXEL_SHADER);
            return AcceptPixelShaderCodeArgument(shaderArgument, source, errorMessage, false);
        }

        bool AcceptShaderSamplerArgument(const techset::ShaderSelector shader, const techset::ShaderArgument shaderArgument, const techset::ShaderArgumentCodeSource source,
                                         std::string& errorMessage) override
        {
            if (shader == techset::ShaderSelector::VERTEX_SHADER)
            {
                errorMessage = "Vertex sampler are unsupported";
                return false;
            }

            assert(shader == techset::ShaderSelector::PIXEL_SHADER);
            return AcceptPixelShaderCodeArgument(shaderArgument, source, errorMessage, true);
        }

        bool AcceptShaderLiteralArgument(const techset::ShaderSelector shader, techset::ShaderArgument shaderArgument, techset::ShaderArgumentLiteralSource source, std::string& errorMessage) override
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            MaterialShaderArgument argument{};
            const d3d9::ShaderInfo* shaderInfo;

            if (shader == techset::ShaderSelector::VERTEX_SHADER)
            {
                argument.type = MTL_ARG_LITERAL_VERTEX_CONST;
                shaderInfo = pass.m_vertex_shader_info.get();
            }
            else
            {
                assert(shader == techset::ShaderSelector::PIXEL_SHADER);
                argument.type = MTL_ARG_LITERAL_PIXEL_CONST;
                shaderInfo = pass.m_pixel_shader_info.get();
            }

            if (!shaderInfo)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            const auto matchingShaderConstant = std::find_if(shaderInfo->m_constants.begin(), shaderInfo->m_constants.end(), [&shaderArgument](const d3d9::ShaderConstant& constant)
            {
                return constant.m_name == shaderArgument.m_argument_name;
            });

            if (matchingShaderConstant == shaderInfo->m_constants.end())
            {
                errorMessage = "Could not find argument in shader";
                return false;
            }

            const auto shaderConstantIndex = static_cast<size_t>(matchingShaderConstant - shaderInfo->m_constants.begin());
            const auto argumentIsSampler = IsSamplerArgument(*matchingShaderConstant);
            if (argumentIsSampler)
            {
                if (shader == techset::ShaderSelector::VERTEX_SHADER)
                    errorMessage = "Vertex shader argument expects sampler but got constant";
                else
                    errorMessage = "Pixel shader argument expects sampler but got constant";

                return false;
            }

            argument.dest = static_cast<uint16_t>(matchingShaderConstant->m_register_index);
            argument.u.literalConst = m_zone_state->GetAllocatedLiteral(m_memory, source);
            pass.m_arguments.push_back(argument);

            if (shader == techset::ShaderSelector::VERTEX_SHADER)
                pass.m_handled_vertex_shader_arguments[shaderConstantIndex] = true;
            else
                pass.m_handled_pixel_shader_arguments[shaderConstantIndex] = true;

            return true;
        }

        bool AcceptShaderMaterialArgument(const techset::ShaderSelector shader, techset::ShaderArgument shaderArgument, const techset::ShaderArgumentMaterialSource source,
                                          std::string& errorMessage) override
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            MaterialShaderArgument argument{};
            const d3d9::ShaderInfo* shaderInfo;

            if (shader == techset::ShaderSelector::VERTEX_SHADER)
            {
                shaderInfo = pass.m_vertex_shader_info.get();
            }
            else
            {
                assert(shader == techset::ShaderSelector::PIXEL_SHADER);
                shaderInfo = pass.m_pixel_shader_info.get();
            }

            if (!shaderInfo)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            const auto matchingShaderConstant = std::find_if(shaderInfo->m_constants.begin(), shaderInfo->m_constants.end(), [&shaderArgument](const d3d9::ShaderConstant& constant)
            {
                return constant.m_name == shaderArgument.m_argument_name;
            });

            if (matchingShaderConstant == shaderInfo->m_constants.end())
            {
                errorMessage = "Could not find argument in shader";
                return false;
            }

            const auto shaderConstantIndex = static_cast<size_t>(matchingShaderConstant - shaderInfo->m_constants.begin());
            const auto argumentIsSampler = IsSamplerArgument(*matchingShaderConstant);
            if (shader == techset::ShaderSelector::VERTEX_SHADER)
            {
                if (argumentIsSampler)
                {
                    errorMessage = "Vertex sampler are unsupported";
                    return false;
                }
                argument.type = MTL_ARG_MATERIAL_VERTEX_CONST;
            }
            else
            {
                assert(shader == techset::ShaderSelector::PIXEL_SHADER);
                argument.type = !argumentIsSampler ? MTL_ARG_MATERIAL_PIXEL_CONST : MTL_ARG_MATERIAL_PIXEL_SAMPLER;
            }

            if (source.m_is_hash)
                argument.u.nameHash = static_cast<unsigned>(source.m_hash);
            else
                argument.u.nameHash = Common::R_HashString(source.m_name.c_str(), 0u);

            argument.dest = static_cast<uint16_t>(matchingShaderConstant->m_register_index);
            pass.m_arguments.push_back(argument);

            if (shader == techset::ShaderSelector::VERTEX_SHADER)
                pass.m_handled_vertex_shader_arguments[shaderConstantIndex] = true;
            else
                pass.m_handled_pixel_shader_arguments[shaderConstantIndex] = true;

            return true;
        }

        bool AcceptVertexStreamRouting(const std::string& destination, const std::string& source, std::string& errorMessage) override
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            const auto streamIndex = static_cast<size_t>(pass.m_vertex_decl.streamCount);
            if (pass.m_vertex_decl.streamCount >= std::extent_v<decltype(MaterialVertexStreamRouting::data)>)
            {
                errorMessage = "Too many stream routings";
                return false;
            }

            const auto foundDestination = std::find(std::begin(materialStreamDestinationNames), std::end(materialStreamDestinationNames), destination);
            if (foundDestination == std::end(materialStreamDestinationNames))
            {
                errorMessage = "Unknown stream destination";
                return false;
            }

            const auto foundSource = std::find(std::begin(materialStreamSourceNames), std::end(materialStreamSourceNames), source);
            if (foundSource == std::end(materialStreamSourceNames))
            {
                errorMessage = "Unknown stream source";
                return false;
            }

            const auto destinationIndex = static_cast<MaterialStreamDestination_e>(foundDestination - std::begin(materialStreamDestinationNames));
            const auto sourceIndex = static_cast<MaterialStreamStreamSource_e>(foundSource - std::begin(materialStreamSourceNames));

            pass.m_vertex_decl.routing.data[streamIndex].dest = destinationIndex;
            pass.m_vertex_decl.routing.data[streamIndex].source = sourceIndex;

            pass.m_vertex_decl.hasOptionalSource = pass.m_vertex_decl.hasOptionalSource || sourceIndex >= STREAM_SRC_OPTIONAL_BEGIN;

            pass.m_vertex_decl.streamCount++;
            return true;
        }
    };

    class TechniqueLoader
    {
        ISearchPath* m_search_path;
        MemoryManager* m_memory;
        IAssetLoadingManager* m_manager;
        TechniqueZoneLoadingState* m_zone_state;

        static std::string GetTechniqueFileName(const std::string& techniqueName)
        {
            std::ostringstream ss;
            ss << "techniques/" << techniqueName << ".tech";
            return ss.str();
        }

        void ConvertPass(MaterialPass& out, const TechniqueCreator::Pass& in)
        {
            out.vertexShader = in.m_vertex_shader->Asset();
            out.pixelShader = in.m_pixel_shader->Asset();
        }

        MaterialTechnique* ConvertTechnique(const std::string& techniqueName, const std::vector<TechniqueCreator::Pass>& passes)
        {
            assert(!passes.empty());
            // TODO: Load technique or use previously loaded one
            const auto techniqueSize = sizeof(MaterialTechnique) + (passes.size() - 1u) * sizeof(MaterialPass);
            auto* technique = static_cast<MaterialTechnique*>(m_memory->Alloc(techniqueSize));
            memset(technique, 0, techniqueSize);
            technique->name = m_memory->Dup(techniqueName.c_str());
            technique->passCount = static_cast<uint16_t>(passes.size());

            for (auto i = 0u; i < passes.size(); i++)
                ConvertPass(technique->passArray[i], passes.at(i));

            return technique;
        }

        MaterialTechnique* LoadTechniqueFromRaw(const std::string& techniqueName, std::vector<XAssetInfoGeneric*>& dependencies)
        {
            const auto techniqueFileName = GetTechniqueFileName(techniqueName);
            const auto file = m_search_path->Open(techniqueFileName);
            if (!file.IsOpen())
                return nullptr;

            TechniqueCreator creator(m_memory, m_manager, m_zone_state);
            const techset::TechniqueFileReader reader(*file.m_stream, techniqueFileName, &creator);
            if (!reader.ReadTechniqueDefinition())
                return nullptr;

            return ConvertTechnique(techniqueName, creator.m_passes);
        }

    public:
        TechniqueLoader(ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
            : m_search_path(searchPath),
              m_memory(memory),
              m_manager(manager),
              m_zone_state(manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<TechniqueZoneLoadingState>())
        {
        }

        const LoadedTechnique* LoadMaterialTechnique(const std::string& techniqueName)
        {
            auto* technique = m_zone_state->FindLoadedTechnique(techniqueName);
            if (technique)
                return technique;

            std::vector<XAssetInfoGeneric*> dependencies;
            auto* techniqueFromRaw = LoadTechniqueFromRaw(techniqueName, dependencies);
            if (techniqueFromRaw == nullptr)
                return nullptr;

            return m_zone_state->AddLoadedTechnique(techniqueName, techniqueFromRaw, dependencies);
        }
    };
}

void* AssetLoaderTechniqueSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* techset = memory->Create<MaterialTechniqueSet>();
    memset(techset, 0, sizeof(MaterialTechniqueSet));
    techset->name = memory->Dup(assetName.c_str());
    return techset;
}

std::string AssetLoaderTechniqueSet::GetTechsetFileName(const std::string& techsetAssetName)
{
    std::ostringstream ss;
    ss << "techsets/" << techsetAssetName << ".techset";
    return ss.str();
}

bool AssetLoaderTechniqueSet::CreateTechsetFromDefinition(const std::string& assetName, const techset::TechsetDefinition& definition, ISearchPath* searchPath, MemoryManager* memory,
                                                          IAssetLoadingManager* manager)
{
    auto* techset = memory->Create<MaterialTechniqueSet>();
    memset(techset, 0, sizeof(MaterialTechniqueSet));
    techset->name = memory->Dup(assetName.c_str());

    TechniqueLoader techniqueLoader(searchPath, memory, manager);
    std::set<XAssetInfoGeneric*> dependencies;
    for (auto i = 0u; i < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; i++)
    {
        std::string techniqueName;
        if (definition.GetTechniqueByIndex(i, techniqueName))
        {
            auto* technique = techniqueLoader.LoadMaterialTechnique(techniqueName);

            if (!technique)
                return false;

            for (auto* techniqueDependency : technique->m_dependencies)
                dependencies.emplace(techniqueDependency);

            techset->techniques[i] = technique->m_technique;
        }
    }

    manager->AddAsset(ASSET_TYPE_TECHNIQUE_SET, assetName, techset, std::vector(dependencies.begin(), dependencies.end()), std::vector<scr_string_t>());

    return true;
}

bool AssetLoaderTechniqueSet::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderTechniqueSet::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto techsetFileName = GetTechsetFileName(assetName);
    const auto file = searchPath->Open(techsetFileName);
    if (!file.IsOpen())
        return false;

    const techset::TechsetFileReader reader(*file.m_stream, techsetFileName, techniqueTypeNames, std::extent_v<decltype(techniqueTypeNames)>);
    const auto techsetDefinition = reader.ReadTechsetDefinition();

    if (techsetDefinition)
        return CreateTechsetFromDefinition(assetName, *techsetDefinition, searchPath, memory, manager);

    return false;
}
