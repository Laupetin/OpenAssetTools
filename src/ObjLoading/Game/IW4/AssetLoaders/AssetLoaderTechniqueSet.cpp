#include "AssetLoaderTechniqueSet.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <type_traits>

#include "AssetLoaderPixelShader.h"
#include "AssetLoaderVertexShader.h"
#include "Utils/ClassUtils.h"
#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Pool/GlobalAssetPool.h"
#include "Techset/TechniqueFileReader.h"
#include "Techset/TechsetFileReader.h"
#include "Shader/D3D9ShaderAnalyser.h"
#include "Utils/Alignment.h"

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

    class ShaderInfoFromFileSystemCacheState final : public IZoneAssetLoaderState
    {
        std::unordered_map<std::string, std::unique_ptr<d3d9::ShaderInfo>> m_cached_shader_info;

    public:
        _NODISCARD const d3d9::ShaderInfo* LoadShaderInfoFromDisk(ISearchPath* searchPath, const std::string& fileName)
        {
            const auto cachedShaderInfo = m_cached_shader_info.find(fileName);
            if (cachedShaderInfo != m_cached_shader_info.end())
                return cachedShaderInfo->second.get();

            const auto file = searchPath->Open(fileName);
            if (!file.IsOpen())
                return nullptr;

            const auto shaderSize = static_cast<size_t>(file.m_length);

            if (shaderSize % sizeof(uint32_t) != 0)
            {
                std::cerr << "Invalid shader \"" << fileName << "\": Size must be dividable by " << sizeof(uint32_t) << "\n";
                return nullptr;
            }

            const auto shaderData = std::make_unique<char[]>(shaderSize);
            file.m_stream->read(shaderData.get(), shaderSize);

            auto shaderInfo = d3d9::ShaderAnalyser::GetShaderInfo(reinterpret_cast<const uint32_t*>(shaderData.get()), shaderSize);
            if (!shaderInfo)
                return nullptr;

            const auto* result = shaderInfo.get();
            m_cached_shader_info.emplace(std::make_pair(fileName, std::move(shaderInfo)));
            return result;
        }
    };

    class TechniqueCreator final : public techset::ITechniqueDefinitionAcceptor
    {
        ISearchPath* const m_search_path;
        MemoryManager* const m_memory;
        IAssetLoadingManager* const m_manager;
        TechniqueZoneLoadingState* const m_zone_state;
        ShaderInfoFromFileSystemCacheState* const m_shader_info_cache;

    public:
        class PassShaderArgument
        {
        public:
            MaterialShaderArgument m_arg;
            MaterialUpdateFrequency m_update_frequency;

            static MaterialUpdateFrequency GetUpdateFrequencyForArg(MaterialShaderArgument arg)
            {
                switch (arg.type)
                {
                case MTL_ARG_CODE_VERTEX_CONST:
                case MTL_ARG_CODE_PIXEL_CONST:
                    if (arg.u.codeConst.index >= std::extent_v<decltype(s_codeConstUpdateFreq)>)
                    {
                        assert(false);
                        return MTL_UPDATE_RARELY;
                    }
                    return s_codeConstUpdateFreq[arg.u.codeConst.index];

                case MTL_ARG_CODE_PIXEL_SAMPLER:
                    if (arg.u.codeSampler >= std::extent_v<decltype(s_codeSamplerUpdateFreq)>)
                    {
                        assert(false);
                        return MTL_UPDATE_RARELY;
                    }
                    return s_codeSamplerUpdateFreq[arg.u.codeSampler];

                case MTL_ARG_MATERIAL_VERTEX_CONST:
                case MTL_ARG_MATERIAL_PIXEL_SAMPLER:
                case MTL_ARG_MATERIAL_PIXEL_CONST:
                case MTL_ARG_LITERAL_VERTEX_CONST:
                case MTL_ARG_LITERAL_PIXEL_CONST:
                default:
                    return MTL_UPDATE_RARELY;
                }
            }

            explicit PassShaderArgument(const MaterialShaderArgument arg)
                : m_arg(arg),
                  m_update_frequency(GetUpdateFrequencyForArg(arg))
            {
            }
        };

        struct Pass
        {
            XAssetInfo<MaterialVertexShader>* m_vertex_shader;
            const d3d9::ShaderInfo* m_vertex_shader_info;
            std::unique_ptr<d3d9::ShaderInfo> m_vertex_shader_info_unq;
            std::vector<size_t> m_vertex_shader_argument_handled_offset;
            std::vector<bool> m_handled_vertex_shader_arguments;

            XAssetInfo<MaterialPixelShader>* m_pixel_shader;
            const d3d9::ShaderInfo* m_pixel_shader_info;
            std::unique_ptr<d3d9::ShaderInfo> m_pixel_shader_info_unq;
            std::vector<size_t> m_pixel_shader_argument_handled_offset;
            std::vector<bool> m_handled_pixel_shader_arguments;

            MaterialVertexDeclaration m_vertex_decl;
            XAssetInfo<MaterialVertexDeclaration>* m_vertex_decl_asset;
            std::vector<PassShaderArgument> m_arguments;

            Pass()
                : m_vertex_shader(nullptr),
                  m_vertex_shader_info(nullptr),
                  m_pixel_shader(nullptr),
                  m_pixel_shader_info(nullptr),
                  m_vertex_decl{},
                  m_vertex_decl_asset(nullptr)
            {
            }
        };

        std::vector<Pass> m_passes;
        std::vector<XAssetInfoGeneric*> m_dependencies;

        TechniqueCreator(ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, TechniqueZoneLoadingState* zoneState, ShaderInfoFromFileSystemCacheState* shaderInfoCache)
            : m_search_path(searchPath),
              m_memory(memory),
              m_manager(manager),
              m_zone_state(zoneState),
              m_shader_info_cache(shaderInfoCache)
        {
        }

        void AcceptNextPass() override
        {
            m_passes.emplace_back();
        }

        static size_t RegisterCountPerElement(const d3d9::ShaderConstant& constant)
        {
            const auto valuesPerRegister = constant.m_register_set == d3d9::RegisterSet::BOOL || constant.m_register_set == d3d9::RegisterSet::SAMPLER ? 1u : 4u;
            return utils::Align(constant.m_type_columns * constant.m_type_rows, valuesPerRegister) / valuesPerRegister;
        }

        static bool IsSamplerArgument(const d3d9::ShaderConstant& constant)
        {
            return constant.m_type == d3d9::ParameterType::SAMPLER
                || constant.m_type == d3d9::ParameterType::SAMPLER_1D
                || constant.m_type == d3d9::ParameterType::SAMPLER_2D
                || constant.m_type == d3d9::ParameterType::SAMPLER_3D
                || constant.m_type == d3d9::ParameterType::SAMPLER_CUBE;
        }

        bool AutoCreateShaderArgument(const techset::ShaderSelector shaderType, const d3d9::ShaderConstant& shaderArgument, const size_t elementOffset, const size_t registerOffset)
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            const auto isSamplerArgument = IsSamplerArgument(shaderArgument);
            if (shaderType == techset::ShaderSelector::VERTEX_SHADER && isSamplerArgument)
                return false;

            MaterialShaderArgument argument{};
            argument.dest = static_cast<uint16_t>(shaderArgument.m_register_index + registerOffset);

            unsigned arrayCount;
            const std::vector accessors({shaderArgument.m_name});
            if (isSamplerArgument)
            {
                const CodeSamplerSource* samplerSource = FindCodeSamplerSource(accessors, s_codeSamplers);
                if (!samplerSource)
                    samplerSource = FindCodeSamplerSource(accessors, s_defaultCodeSamplers);

                if (!samplerSource)
                    return false;

                argument.type = MTL_ARG_CODE_PIXEL_SAMPLER;
                argument.u.codeSampler = samplerSource->source + elementOffset;

                arrayCount = static_cast<unsigned>(samplerSource->arrayCount);
            }
            else
            {
                const CodeConstantSource* constantSource = FindCodeConstantSource(accessors, s_codeConsts);
                if (!constantSource)
                    constantSource = FindCodeConstantSource(accessors, s_defaultCodeConsts);

                if (!constantSource)
                    return false;

                argument.type = shaderType == techset::ShaderSelector::VERTEX_SHADER ? MTL_ARG_CODE_VERTEX_CONST : MTL_ARG_CODE_PIXEL_CONST;
                argument.u.codeConst.index = static_cast<uint16_t>(constantSource->source + elementOffset);
                argument.u.codeConst.firstRow = 0u;
                argument.u.codeConst.rowCount = static_cast<unsigned char>(shaderArgument.m_type_rows);

                arrayCount = static_cast<unsigned>(constantSource->arrayCount);
            }

            if (elementOffset >= std::max(arrayCount, 1u))
                return false;

            pass.m_arguments.emplace_back(argument);
            return true;
        }

        bool AutoCreateVertexShaderArguments(std::string& errorMessage)
        {
            assert(!m_passes.empty());
            const auto& pass = m_passes.at(m_passes.size() - 1);

            for (auto i = 0u; i < pass.m_vertex_shader_argument_handled_offset.size(); i++)
            {
                const auto& argument = pass.m_vertex_shader_info->m_constants[i];
                const auto argumentHandledIndex = pass.m_vertex_shader_argument_handled_offset[i];
                const auto argumentRegistersPerElement = argument.m_type_elements > 1u ? RegisterCountPerElement(argument) : argument.m_register_count;

                auto elementIndex = 0u;
                for (auto registerIndex = 0u; registerIndex < argument.m_register_count; registerIndex += argumentRegistersPerElement)
                {
                    if (!pass.m_handled_vertex_shader_arguments[argumentHandledIndex + elementIndex])
                    {
                        if (!AutoCreateShaderArgument(techset::ShaderSelector::VERTEX_SHADER, argument, elementIndex, registerIndex))
                        {
                            std::ostringstream ss;
                            ss << "Unassigned vertex shader \"" << pass.m_vertex_shader->m_name << "\" arg: " << argument.m_name;

                            if (argument.m_type_elements > 1)
                                ss << '[' << elementIndex << ']';

                            errorMessage = ss.str();
                            return false;
                        }
                    }

                    elementIndex++;
                }
            }

            return true;
        }

        bool AutoCreatePixelShaderArguments(std::string& errorMessage)
        {
            assert(!m_passes.empty());
            const auto& pass = m_passes.at(m_passes.size() - 1);

            for (auto i = 0u; i < pass.m_pixel_shader_argument_handled_offset.size(); i++)
            {
                const auto& argument = pass.m_pixel_shader_info->m_constants[i];
                const auto argumentHandledIndex = pass.m_pixel_shader_argument_handled_offset[i];
                const auto argumentRegistersPerElement = argument.m_type_elements > 1u ? RegisterCountPerElement(argument) : argument.m_register_count;

                auto elementIndex = 0u;
                for (auto registerIndex = 0u; registerIndex < argument.m_register_count; registerIndex += argumentRegistersPerElement)
                {
                    if (!pass.m_handled_pixel_shader_arguments[argumentHandledIndex + elementIndex])
                    {
                        if (!AutoCreateShaderArgument(techset::ShaderSelector::PIXEL_SHADER, argument, elementIndex, registerIndex))
                        {
                            std::ostringstream ss;
                            ss << "Unassigned pixel shader \"" << pass.m_pixel_shader->m_name << "\" arg: " << argument.m_name;

                            if (argument.m_type_elements > 1)
                                ss << '[' << elementIndex << ']';

                            errorMessage = ss.str();
                            return false;
                        }
                    }

                    elementIndex++;
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
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            if (!AutoCreateVertexShaderArguments(errorMessage) || !AutoCreatePixelShaderArguments(errorMessage))
                return false;

            // Sort args by their update frequency
            std::sort(pass.m_arguments.begin(), pass.m_arguments.end(), [](const PassShaderArgument& arg1, const PassShaderArgument& arg2)
            {
                if(arg1.m_update_frequency != arg2.m_update_frequency)
                    return arg1.m_update_frequency < arg2.m_update_frequency;

                if (arg1.m_arg.type != arg2.m_arg.type)
                    return arg1.m_arg.type < arg2.m_arg.type;

                if (arg1.m_arg.type == MTL_ARG_MATERIAL_VERTEX_CONST || arg1.m_arg.type == MTL_ARG_MATERIAL_PIXEL_CONST || arg1.m_arg.type == MTL_ARG_MATERIAL_PIXEL_SAMPLER)
                    return arg1.m_arg.u.codeSampler < arg2.m_arg.u.codeSampler;

                return arg1.m_arg.dest < arg2.m_arg.dest;
            });

            AllocateVertexDecl();

            return true;
        }

        void AcceptStateMap(const std::string& stateMapName) override
        {
            // TODO: State maps currently are not used
        }

        static void InitializeArgumentState(const d3d9::ShaderInfo& shaderInfo, std::vector<size_t>& argumentHandledOffsetVector, std::vector<bool>& argumentHandledVector)
        {
            auto vertexShaderArgumentSlotCount = 0u;
            auto argIndex = 0u;
            argumentHandledOffsetVector.resize(shaderInfo.m_constants.size());
            for (const auto& arg : shaderInfo.m_constants)
            {
                argumentHandledOffsetVector[argIndex++] = vertexShaderArgumentSlotCount;

                if (arg.m_type_elements > 1)
                    vertexShaderArgumentSlotCount += arg.m_register_count / RegisterCountPerElement(arg);
                else
                    vertexShaderArgumentSlotCount++;
            }
            argumentHandledVector.resize(vertexShaderArgumentSlotCount);
        }

        bool AcceptVertexShader(const std::string& vertexShaderName, std::string& errorMessage) override
        {
            auto* vertexShaderDependency = m_manager->LoadDependency(ASSET_TYPE_VERTEXSHADER, vertexShaderName);
            if (vertexShaderDependency == nullptr)
            {
                std::ostringstream ss;
                ss << "Failed to load specified shader \"" << vertexShaderName << "\"";
                errorMessage = ss.str();
                return false;
            }

            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);
            pass.m_vertex_shader = reinterpret_cast<XAssetInfo<MaterialVertexShader>*>(vertexShaderDependency);

            if (pass.m_vertex_shader->Asset()->name && pass.m_vertex_shader->Asset()->name[0] == ',')
            {
                pass.m_vertex_shader_info = m_shader_info_cache->LoadShaderInfoFromDisk(m_search_path, AssetLoaderVertexShader::GetFileNameForAsset(vertexShaderName));
            }
            else
            {
                const auto& shaderLoadDef = pass.m_vertex_shader->Asset()->prog.loadDef;
                pass.m_vertex_shader_info_unq = d3d9::ShaderAnalyser::GetShaderInfo(shaderLoadDef.program, shaderLoadDef.programSize * sizeof(uint32_t));
                pass.m_vertex_shader_info = pass.m_vertex_shader_info_unq.get();
            }

            if (!pass.m_vertex_shader_info)
            {
                std::ostringstream ss;
                ss << "No shader info for shader \"" << vertexShaderName << "\"";
                errorMessage = ss.str();
                return false;
            }

            InitializeArgumentState(*pass.m_vertex_shader_info, pass.m_vertex_shader_argument_handled_offset, pass.m_handled_vertex_shader_arguments);

            return true;
        }

        bool AcceptPixelShader(const std::string& pixelShaderName, std::string& errorMessage) override
        {
            auto* pixelShaderDependency = m_manager->LoadDependency(ASSET_TYPE_PIXELSHADER, pixelShaderName);
            if (pixelShaderDependency == nullptr)
            {
                std::ostringstream ss;
                ss << "Failed to load specified shader \"" << pixelShaderName << "\"";
                errorMessage = ss.str();
                return false;
            }

            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);
            pass.m_pixel_shader = reinterpret_cast<XAssetInfo<MaterialPixelShader>*>(pixelShaderDependency);

            if (pass.m_pixel_shader->Asset()->name && pass.m_pixel_shader->Asset()->name[0] == ',')
            {
                pass.m_pixel_shader_info = m_shader_info_cache->LoadShaderInfoFromDisk(m_search_path, AssetLoaderPixelShader::GetFileNameForAsset(pixelShaderName));
            }
            else
            {
                const auto& shaderLoadDef = pass.m_pixel_shader->Asset()->prog.loadDef;
                pass.m_pixel_shader_info_unq = d3d9::ShaderAnalyser::GetShaderInfo(shaderLoadDef.program, shaderLoadDef.programSize * sizeof(uint32_t));
                pass.m_pixel_shader_info = pass.m_pixel_shader_info_unq.get();
            }

            if (!pass.m_pixel_shader_info)
            {
                std::ostringstream ss;
                ss << "No shader info for shader \"" << pixelShaderName << "\"";
                errorMessage = ss.str();
                return false;
            }

            InitializeArgumentState(*pass.m_pixel_shader_info, pass.m_pixel_shader_argument_handled_offset, pass.m_handled_pixel_shader_arguments);

            return true;
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

        bool FindShaderArgument(const d3d9::ShaderInfo& shaderInfo, const techset::ShaderArgument& argument, size_t& constantIndex, size_t& registerOffset, std::string& errorMessage) const
        {
            const auto matchingShaderConstant = std::find_if(shaderInfo.m_constants.begin(), shaderInfo.m_constants.end(), [argument](const d3d9::ShaderConstant& constant)
            {
                return constant.m_name == argument.m_argument_name;
            });

            if (matchingShaderConstant == shaderInfo.m_constants.end())
            {
                errorMessage = "Could not find argument in shader";
                return false;
            }

            if (argument.m_argument_index_specified)
            {
                if (matchingShaderConstant->m_type_elements <= 1)
                {
                    errorMessage = "Argument does not have more than one element";
                    return false;
                }

                const auto registersPerElement = RegisterCountPerElement(*matchingShaderConstant);
                if (argument.m_argument_index >= matchingShaderConstant->m_register_count / registersPerElement)
                {
                    errorMessage = "Argument index out of bounds";
                    return false;
                }

                registerOffset = argument.m_argument_index * registersPerElement;
            }
            else if (matchingShaderConstant->m_type_elements > 1)
            {
                errorMessage = "Argument has more than one element and needs to be accessed with an index";
                return false;
            }
            else
            {
                registerOffset = 0u;
            }

            constantIndex = static_cast<size_t>(matchingShaderConstant - shaderInfo.m_constants.begin());
            return true;
        }

        static bool SetArgumentCodeConst(MaterialShaderArgument& argument, const techset::ShaderArgumentCodeSource& source, const d3d9::ShaderConstant& shaderConstant, const unsigned sourceIndex,
                                         const unsigned arrayCount, std::string& errorMessage)
        {
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
            argument.u.codeConst.rowCount = static_cast<unsigned char>(shaderConstant.m_type_rows);

            return true;
        }

        static bool SetArgumentCodeSampler(MaterialShaderArgument& argument, const techset::ShaderArgumentCodeSource& source, const d3d9::ShaderConstant& shaderConstant, const unsigned sourceIndex,
                                           const unsigned arrayCount, std::string& errorMessage)
        {
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

                argument.u.codeSampler = sourceIndex + source.m_index_accessor;
            }
            else if (source.m_index_accessor_specified)
            {
                errorMessage = "Code constant cannot have array index specified";
                return false;
            }
            else
            {
                argument.u.codeSampler = sourceIndex;
            }

            return true;
        }

        bool AcceptVertexShaderConstantArgument(const techset::ShaderArgument& shaderArgument, const techset::ShaderArgumentCodeSource& source, std::string& errorMessage)
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            if (!pass.m_vertex_shader_info)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            size_t shaderConstantIndex = 0u;
            size_t registerOffset = 0u;
            if (!FindShaderArgument(*pass.m_vertex_shader_info, shaderArgument, shaderConstantIndex, registerOffset, errorMessage))
                return false;

            const auto elementOffset = shaderArgument.m_argument_index_specified ? shaderArgument.m_argument_index : 0u;
            const auto& shaderConstant = pass.m_vertex_shader_info->m_constants[shaderConstantIndex];
            const auto argumentIsSampler = IsSamplerArgument(shaderConstant);
            if (argumentIsSampler)
            {
                errorMessage = "Vertex shader argument expected sampler but got constant";
                return false;
            }

            MaterialShaderArgument argument{};
            argument.type = MTL_ARG_CODE_VERTEX_CONST;
            argument.dest = static_cast<uint16_t>(shaderConstant.m_register_index + registerOffset);

            const CodeConstantSource* constantSource = FindCodeConstantSource(source.m_accessors, s_codeConsts);
            if (!constantSource)
                constantSource = FindCodeConstantSource(source.m_accessors, s_defaultCodeConsts);

            if (!constantSource)
            {
                errorMessage = "Unknown code constant";
                return false;
            }

            if (!SetArgumentCodeConst(argument, source, shaderConstant, constantSource->source, constantSource->arrayCount, errorMessage))
                return false;

            pass.m_arguments.emplace_back(argument);
            pass.m_handled_vertex_shader_arguments[pass.m_vertex_shader_argument_handled_offset[shaderConstantIndex] + elementOffset] = true;

            return true;
        }

        bool AcceptPixelShaderCodeArgument(const techset::ShaderArgument& shaderArgument, const techset::ShaderArgumentCodeSource& source, std::string& errorMessage, const bool isSampler)
        {
            assert(!m_passes.empty());
            auto& pass = m_passes.at(m_passes.size() - 1);

            if (!pass.m_pixel_shader_info)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            size_t shaderConstantIndex = 0u;
            size_t registerOffset = 0u;
            if (!FindShaderArgument(*pass.m_pixel_shader_info, shaderArgument, shaderConstantIndex, registerOffset, errorMessage))
                return false;

            const auto elementOffset = shaderArgument.m_argument_index_specified ? shaderArgument.m_argument_index : 0u;
            const auto& shaderConstant = pass.m_pixel_shader_info->m_constants[shaderConstantIndex];
            const auto argumentIsSampler = IsSamplerArgument(shaderConstant);
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
            argument.dest = static_cast<uint16_t>(shaderConstant.m_register_index + registerOffset);

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

                if (!SetArgumentCodeSampler(argument, source, shaderConstant, sourceIndex, arrayCount, errorMessage))
                    return false;
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

                if (!SetArgumentCodeConst(argument, source, shaderConstant, sourceIndex, arrayCount, errorMessage))
                    return false;
            }


            pass.m_arguments.emplace_back(argument);
            pass.m_handled_pixel_shader_arguments[pass.m_pixel_shader_argument_handled_offset[shaderConstantIndex] + elementOffset] = true;

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
                shaderInfo = pass.m_vertex_shader_info;
            }
            else
            {
                assert(shader == techset::ShaderSelector::PIXEL_SHADER);
                argument.type = MTL_ARG_LITERAL_PIXEL_CONST;
                shaderInfo = pass.m_pixel_shader_info;
            }

            if (!shaderInfo)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            size_t shaderConstantIndex = 0u;
            size_t registerOffset = 0u;
            if (!FindShaderArgument(*shaderInfo, shaderArgument, shaderConstantIndex, registerOffset, errorMessage))
                return false;

            const auto elementOffset = shaderArgument.m_argument_index_specified ? shaderArgument.m_argument_index : 0u;
            const auto& shaderConstant = shaderInfo->m_constants[shaderConstantIndex];
            const auto argumentIsSampler = IsSamplerArgument(shaderConstant);
            if (argumentIsSampler)
            {
                if (shader == techset::ShaderSelector::VERTEX_SHADER)
                    errorMessage = "Vertex shader argument expects sampler but got constant";
                else
                    errorMessage = "Pixel shader argument expects sampler but got constant";

                return false;
            }

            argument.dest = static_cast<uint16_t>(shaderConstant.m_register_index + registerOffset);
            argument.u.literalConst = m_zone_state->GetAllocatedLiteral(m_memory, source);
            pass.m_arguments.emplace_back(argument);

            if (shader == techset::ShaderSelector::VERTEX_SHADER)
                pass.m_handled_vertex_shader_arguments[pass.m_vertex_shader_argument_handled_offset[shaderConstantIndex] + elementOffset] = true;
            else
                pass.m_handled_pixel_shader_arguments[pass.m_pixel_shader_argument_handled_offset[shaderConstantIndex] + elementOffset] = true;

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
                shaderInfo = pass.m_vertex_shader_info;
            }
            else
            {
                assert(shader == techset::ShaderSelector::PIXEL_SHADER);
                shaderInfo = pass.m_pixel_shader_info;
            }

            if (!shaderInfo)
            {
                errorMessage = "Shader not specified";
                return false;
            }

            size_t shaderConstantIndex = 0u;
            size_t registerOffset = 0u;
            if (!FindShaderArgument(*shaderInfo, shaderArgument, shaderConstantIndex, registerOffset, errorMessage))
                return false;

            const auto elementOffset = shaderArgument.m_argument_index_specified ? shaderArgument.m_argument_index : 0u;
            const auto& shaderConstant = shaderInfo->m_constants[shaderConstantIndex];
            const auto argumentIsSampler = IsSamplerArgument(shaderConstant);
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

            argument.dest = static_cast<uint16_t>(shaderConstant.m_register_index + registerOffset);
            pass.m_arguments.emplace_back(argument);

            if (shader == techset::ShaderSelector::VERTEX_SHADER)
                pass.m_handled_vertex_shader_arguments[pass.m_vertex_shader_argument_handled_offset[shaderConstantIndex] + elementOffset] = true;
            else
                pass.m_handled_pixel_shader_arguments[pass.m_pixel_shader_argument_handled_offset[shaderConstantIndex] + elementOffset] = true;

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
        ShaderInfoFromFileSystemCacheState* m_shader_info_cache;

        static std::string GetTechniqueFileName(const std::string& techniqueName)
        {
            std::ostringstream ss;
            ss << "techniques/" << techniqueName << ".tech";
            return ss.str();
        }

        void ConvertPass(MaterialPass& out, const TechniqueCreator::Pass& in, std::vector<XAssetInfoGeneric*>& dependencies) const
        {
            out.customSamplerFlags = 0u;
            out.vertexShader = in.m_vertex_shader->Asset();
            out.pixelShader = in.m_pixel_shader->Asset();
            out.vertexDecl = in.m_vertex_decl_asset->Asset();

            const auto argDataSize = sizeof(MaterialShaderArgument) * in.m_arguments.size();
            out.args = static_cast<MaterialShaderArgument*>(m_memory->Alloc(argDataSize));

            size_t perObjArgCount = 0u;
            size_t perPrimArgCount = 0u;
            size_t stableArgCount = 0u;
            size_t argIndex = 0u;
            for (const auto& arg : in.m_arguments)
            {
                switch (arg.m_update_frequency)
                {
                case MTL_UPDATE_PER_PRIM:
                    perPrimArgCount++;
                    break;
                case MTL_UPDATE_PER_OBJECT:
                    perObjArgCount++;
                    break;
                case MTL_UPDATE_RARELY:
                    stableArgCount++;
                    break;
                case MTL_UPDATE_CUSTOM:
                    {
                        assert(arg.m_arg.type == MTL_ARG_CODE_PIXEL_SAMPLER);
                        if (arg.m_arg.type == MTL_ARG_CODE_PIXEL_SAMPLER)
                        {
                            const auto customSampler = std::find(std::begin(g_customSamplerSrc), std::end(g_customSamplerSrc), arg.m_arg.u.codeSampler);
                            assert(customSampler != std::end(g_customSamplerSrc));
                            if (customSampler != std::end(g_customSamplerSrc))
                            {
                                const auto customSamplerIndex = customSampler - std::begin(g_customSamplerSrc);
                                out.customSamplerFlags |= 1 << customSamplerIndex;
                            }
                        }
                    }
                    continue;
                default:
                    assert(false);
                    continue;
                }

                out.args[argIndex++] = arg.m_arg;
            }

            out.perObjArgCount = static_cast<unsigned char>(perObjArgCount);
            out.perPrimArgCount = static_cast<unsigned char>(perPrimArgCount);
            out.stableArgCount = static_cast<unsigned char>(stableArgCount);

            if (in.m_vertex_shader)
                dependencies.push_back(in.m_vertex_shader);
            if (in.m_pixel_shader)
                dependencies.push_back(in.m_pixel_shader);
            if (in.m_vertex_decl_asset)
                dependencies.push_back(in.m_vertex_decl_asset);
        }

        MaterialTechnique* ConvertTechnique(const std::string& techniqueName, const std::vector<TechniqueCreator::Pass>& passes, std::vector<XAssetInfoGeneric*>& dependencies) const
        {
            assert(!passes.empty());
            const auto techniqueSize = sizeof(MaterialTechnique) + (passes.size() - 1u) * sizeof(MaterialPass);
            auto* technique = static_cast<MaterialTechnique*>(m_memory->Alloc(techniqueSize));
            memset(technique, 0, techniqueSize);
            technique->name = m_memory->Dup(techniqueName.c_str());
            technique->passCount = static_cast<uint16_t>(passes.size());

            for (auto i = 0u; i < passes.size(); i++)
                ConvertPass(technique->passArray[i], passes.at(i), dependencies);

            return technique;
        }

        MaterialTechnique* LoadTechniqueFromRaw(const std::string& techniqueName, std::vector<XAssetInfoGeneric*>& dependencies) const
        {
            const auto techniqueFileName = GetTechniqueFileName(techniqueName);
            const auto file = m_search_path->Open(techniqueFileName);
            if (!file.IsOpen())
                return nullptr;

            TechniqueCreator creator(m_search_path, m_memory, m_manager, m_zone_state, m_shader_info_cache);
            const techset::TechniqueFileReader reader(*file.m_stream, techniqueFileName, &creator);
            if (!reader.ReadTechniqueDefinition())
                return nullptr;

            return ConvertTechnique(techniqueName, creator.m_passes, dependencies);
        }

    public:
        TechniqueLoader(ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
            : m_search_path(searchPath),
              m_memory(memory),
              m_manager(manager),
              m_zone_state(manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<TechniqueZoneLoadingState>()),
              m_shader_info_cache(manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<ShaderInfoFromFileSystemCacheState>())
        {
        }

        _NODISCARD const LoadedTechnique* LoadMaterialTechnique(const std::string& techniqueName) const
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

    const TechniqueLoader techniqueLoader(searchPath, memory, manager);
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
