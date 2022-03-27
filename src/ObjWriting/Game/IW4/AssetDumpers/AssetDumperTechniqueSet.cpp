#include "AssetDumperTechniqueSet.h"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <type_traits>

#include "Dumping/AbstractTextDumper.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Pool/GlobalAssetPool.h"
#include "Shader/D3D9ShaderAnalyser.h"

using namespace IW4;

namespace IW4
{
    class TechniqueDumpingZoneState final : public IZoneAssetDumperState
    {
        std::set<const MaterialTechnique*> m_dumped_techniques;

    public:
        bool ShouldDumpTechnique(const MaterialTechnique* technique)
        {
            if (m_dumped_techniques.find(technique) != m_dumped_techniques.end())
                return false;

            m_dumped_techniques.emplace(technique);
            return true;
        }
    };

    class TechniqueFileWriter : public AbstractTextDumper
    {
        void DumpStateMap() const
        {
            Indent();
            // TODO: Actual statemap: Maybe find all materials using this techset and try to make out rules for the flags based on the statebitstable
            m_stream << "stateMap \"\"; // TODO\n";
        }

        static bool FindCodeConstantSourceAccessor(const MaterialConstantSource sourceIndexToFind, const CodeConstantSource* codeConstantTable, std::string& codeSourceAccessor)
        {
            const auto* currentCodeConst = codeConstantTable;
            while (currentCodeConst->name != nullptr)
            {
                if (currentCodeConst->subtable != nullptr)
                {
                    std::string accessorInSubTable;
                    if (FindCodeConstantSourceAccessor(sourceIndexToFind, currentCodeConst->subtable, accessorInSubTable))
                    {
                        std::ostringstream ss;
                        ss << currentCodeConst->name << '.' << accessorInSubTable;
                        codeSourceAccessor = ss.str();
                        return true;
                    }
                }
                else if (currentCodeConst->arrayCount > 0)
                {
                    if (currentCodeConst->source <= static_cast<unsigned>(sourceIndexToFind)
                        && static_cast<unsigned>(currentCodeConst->source) + currentCodeConst->arrayCount > static_cast<unsigned>(sourceIndexToFind))
                    {
                        std::ostringstream ss;
                        ss << currentCodeConst->name << '[' << (static_cast<unsigned>(sourceIndexToFind) - static_cast<unsigned>(currentCodeConst->source)) << ']';
                        codeSourceAccessor = ss.str();
                        return true;
                    }
                }
                else if (currentCodeConst->source == sourceIndexToFind)
                {
                    codeSourceAccessor = currentCodeConst->name;
                    return true;
                }

                currentCodeConst++;
            }

            return false;
        }

        static bool FindCodeSamplerSourceAccessor(const MaterialTextureSource sourceIndexToFind, const CodeSamplerSource* codeSamplerTable, std::string& codeSourceAccessor)
        {
            const auto* currentCodeConst = codeSamplerTable;
            while (currentCodeConst->name != nullptr)
            {
                if (currentCodeConst->subtable != nullptr)
                {
                    std::string accessorInSubTable;
                    if (FindCodeSamplerSourceAccessor(sourceIndexToFind, currentCodeConst->subtable, accessorInSubTable))
                    {
                        std::ostringstream ss;
                        ss << currentCodeConst->name << '.' << accessorInSubTable;
                        codeSourceAccessor = ss.str();
                        return true;
                    }
                }
                else if (currentCodeConst->arrayCount > 0)
                {
                    if (currentCodeConst->source <= sourceIndexToFind
                        && static_cast<unsigned>(currentCodeConst->source) + currentCodeConst->arrayCount > static_cast<unsigned>(sourceIndexToFind))
                    {
                        std::ostringstream ss;
                        ss << currentCodeConst->name << '[' << (static_cast<unsigned>(sourceIndexToFind) - static_cast<unsigned>(currentCodeConst->source)) << ']';
                        codeSourceAccessor = ss.str();
                        return true;
                    }
                }
                else if (currentCodeConst->source == sourceIndexToFind)
                {
                    codeSourceAccessor = currentCodeConst->name;
                    return true;
                }

                currentCodeConst++;
            }

            return false;
        }

        void DumpShaderArg(const MaterialShaderArgument& arg, const d3d9::ShaderInfo& shaderInfo) const
        {
            const auto targetShaderArg = std::find_if(shaderInfo.m_constants.begin(), shaderInfo.m_constants.end(), [arg](const d3d9::ShaderConstant& constant)
            {
                return constant.m_register_index <= arg.dest && constant.m_register_index + constant.m_register_count > arg.dest;
            });

            assert(targetShaderArg != shaderInfo.m_constants.end());
            if (targetShaderArg == shaderInfo.m_constants.end())
            {
                m_stream << "// Unrecognized arg dest:" << arg.dest << " type: " << arg.type << "\n";
                return;
            }

            std::string codeDestAccessor;
            if (targetShaderArg->m_type_elements > 1)
            {
                std::ostringstream ss;
                ss << targetShaderArg->m_name << '[' << (arg.dest - targetShaderArg->m_register_index) << ']';
                codeDestAccessor = ss.str();
            }
            else
                codeDestAccessor = targetShaderArg->m_name;

            if (arg.type == MTL_ARG_CODE_VERTEX_CONST || arg.type == MTL_ARG_CODE_PIXEL_CONST)
            {
                const auto sourceIndex = static_cast<MaterialConstantSource>(arg.u.codeConst.index);
                std::string codeSourceAccessor;
                if (FindCodeConstantSourceAccessor(sourceIndex, s_codeConsts, codeSourceAccessor)
                    || FindCodeConstantSourceAccessor(sourceIndex, s_defaultCodeConsts, codeSourceAccessor))
                {
                    if (codeDestAccessor != codeSourceAccessor)
                    {
                        Indent();
                        m_stream << codeDestAccessor << " = code." << codeSourceAccessor << ";\n";
                    }
                    else
                    {
#ifdef TECHSET_DEBUG
                        Indent();
                        m_stream << "// Omitted due to matching accessors: " << codeDestAccessor << " = code." << codeSourceAccessor << ";\n";
#endif
                    }
                }
                else
                {
                    assert(false);
                    Indent();
                    m_stream << codeDestAccessor << " = UNKNOWN;\n";
                }
            }
            else if (arg.type == MTL_ARG_CODE_PIXEL_SAMPLER)
            {
                const auto sourceIndex = static_cast<MaterialTextureSource>(arg.u.codeSampler);
                std::string codeSourceAccessor;
                if (FindCodeSamplerSourceAccessor(sourceIndex, s_codeSamplers, codeSourceAccessor)
                    || FindCodeSamplerSourceAccessor(sourceIndex, s_defaultCodeSamplers, codeSourceAccessor))
                {
                    if (codeDestAccessor != codeSourceAccessor)
                    {
                        Indent();
                        m_stream << codeDestAccessor << " = code." << codeSourceAccessor << ";\n";
                    }
                    else
                    {
#ifdef TECHSET_DEBUG
                        Indent();
                        m_stream << "// Omitted due to matching accessors: " << codeDestAccessor << " = code." << codeSourceAccessor << ";\n";
#endif
                    }
                }
                else
                {
                    assert(false);
                    Indent();
                    m_stream << codeDestAccessor << " = UNKNOWN;\n";
                }
            }
            else if (arg.type == MTL_ARG_LITERAL_VERTEX_CONST || arg.type == MTL_ARG_LITERAL_PIXEL_CONST)
            {
                if (arg.u.literalConst)
                {
                    Indent();
                    m_stream << codeDestAccessor << " = float4( " << (*arg.u.literalConst)[0]
                        << ", " << (*arg.u.literalConst)[1]
                        << ", " << (*arg.u.literalConst)[2]
                        << ", " << (*arg.u.literalConst)[3]
                        << " );\n";
                }
            }
            else if (arg.type == MTL_ARG_MATERIAL_PIXEL_CONST || arg.type == MTL_ARG_MATERIAL_VERTEX_CONST || arg.type == MTL_ARG_MATERIAL_PIXEL_SAMPLER)
            {
                Indent();
                m_stream << codeDestAccessor << " = material.#0x" << std::hex << arg.u.nameHash << ";\n";
            }
            else
            {
                assert(false);
            }
        }

        void DumpVertexShader(const MaterialPass& pass)
        {
            auto vertexShader = pass.vertexShader;

            if (vertexShader == nullptr || vertexShader->name == nullptr)
                return;

            if (vertexShader->name[0] == ',')
            {
                const auto loadedVertexShaderFromOtherZone = GlobalAssetPool<MaterialVertexShader>::GetAssetByName(&vertexShader->name[1]);

                if(loadedVertexShaderFromOtherZone == nullptr)
                {
                    // Cannot dump when shader is referenced due to unknown constant names and unknown version
                    Indent();
                    std::cerr << "Cannot dump vertex shader " << &vertexShader->name[1] << " due to being a referenced asset\n";
                    m_stream << "// Cannot dump vertex shader " << &vertexShader->name[1] << " due to being a referenced asset\n";
                    return;
                }
                vertexShader = loadedVertexShaderFromOtherZone->Asset();
            }

            const auto vertexShaderInfo = d3d9::ShaderAnalyser::GetShaderInfo(vertexShader->prog.loadDef.program, vertexShader->prog.loadDef.programSize * sizeof(uint32_t));
            assert(vertexShaderInfo);
            if (!vertexShaderInfo)
                return;

            m_stream << "\n";
            Indent();
            m_stream << "vertexShader " << vertexShaderInfo->m_version_major << "." << vertexShaderInfo->m_version_minor << " \"" << vertexShader->name << "\"\n";
            Indent();
            m_stream << "{\n";
            IncIndent();

            if (pass.args)
            {
                const auto totalArgCount = static_cast<size_t>(pass.perPrimArgCount)
                    + static_cast<size_t>(pass.perObjArgCount)
                    + static_cast<size_t>(pass.stableArgCount);
                for (auto i = 0u; i < totalArgCount; i++)
                {
                    const auto& arg = pass.args[i];
                    if (arg.type == MTL_ARG_MATERIAL_VERTEX_CONST
                        || arg.type == MTL_ARG_LITERAL_VERTEX_CONST
                        || arg.type == MTL_ARG_CODE_VERTEX_CONST)
                    {
                        DumpShaderArg(arg, *vertexShaderInfo);
                    }
                }
            }

            DecIndent();
            Indent();
            m_stream << "}\n";
        }

        void DumpPixelShader(const MaterialPass& pass)
        {
            auto pixelShader = pass.pixelShader;

            if (pixelShader == nullptr || pixelShader->name == nullptr)
                return;

            if (pixelShader->name[0] == ',')
            {
                const auto loadedPixelShaderFromOtherZone = GlobalAssetPool<MaterialPixelShader>::GetAssetByName(&pixelShader->name[1]);

                if (loadedPixelShaderFromOtherZone == nullptr)
                {
                    // Cannot dump when shader is referenced due to unknown constant names and unknown version
                    Indent();
                    std::cerr << "Cannot dump pixel shader " << &pixelShader->name[1] << " due to being a referenced asset\n";
                    m_stream << "// Cannot dump pixel shader " << &pixelShader->name[1] << " due to being a referenced asset\n";
                    return;
                }
                pixelShader = loadedPixelShaderFromOtherZone->Asset();
            }

            const auto pixelShaderInfo = d3d9::ShaderAnalyser::GetShaderInfo(pixelShader->prog.loadDef.program, pixelShader->prog.loadDef.programSize * sizeof(uint32_t));
            assert(pixelShaderInfo);
            if (!pixelShaderInfo)
                return;

            m_stream << "\n";
            Indent();
            m_stream << "pixelShader " << pixelShaderInfo->m_version_major << "." << pixelShaderInfo->m_version_minor << " \"" << pixelShader->name << "\"\n";
            Indent();
            m_stream << "{\n";
            IncIndent();

            if (pass.args)
            {
                const auto totalArgCount = static_cast<size_t>(pass.perPrimArgCount)
                    + static_cast<size_t>(pass.perObjArgCount)
                    + static_cast<size_t>(pass.stableArgCount);
                for (auto i = 0u; i < totalArgCount; i++)
                {
                    const auto& arg = pass.args[i];
                    if (arg.type == MTL_ARG_MATERIAL_PIXEL_SAMPLER
                        || arg.type == MTL_ARG_CODE_PIXEL_SAMPLER
                        || arg.type == MTL_ARG_CODE_PIXEL_CONST
                        || arg.type == MTL_ARG_MATERIAL_PIXEL_CONST
                        || arg.type == MTL_ARG_LITERAL_PIXEL_CONST)
                    {
                        DumpShaderArg(arg, *pixelShaderInfo);
                    }
                }
            }

            DecIndent();
            Indent();
            m_stream << "}\n";
        }

        static const char* GetStreamDestinationString(const MaterialStreamDestination_e dst)
        {
            static const char* destinationNames[]
            {
                "position",
                "normal",
                "color[0]",
                "color[1]",
                "depth",
                "texcoord[0]",
                "texcoord[1]",
                "texcoord[2]",
                "texcoord[3]",
                "texcoord[4]",
                "texcoord[5]",
                "texcoord[6]",
                "texcoord[7]",
            };
            static_assert(std::extent_v<decltype(destinationNames)> == STREAM_DST_COUNT);

            const auto dstIndex = static_cast<size_t>(dst);
            assert(dstIndex < std::extent_v<decltype(destinationNames)>);
            if (dstIndex < std::extent_v<decltype(destinationNames)>)
                return destinationNames[dstIndex];
            return "";
        }

        static const char* GetStreamSourceString(const MaterialStreamStreamSource_e src)
        {
            static const char* sourceNames[]
            {
                "position",
                "color",
                "texcoord[0]",
                "normal",
                "tangent",
                "texcoord[1]",
                "texcoord[2]",
                "normalTransform[0]",
                "normalTransform[1]"
            };
            static_assert(std::extent_v<decltype(sourceNames)> == STREAM_SRC_COUNT);

            const auto srcIndex = static_cast<size_t>(src);
            assert(srcIndex < std::extent_v<decltype(sourceNames)>);
            if (srcIndex < std::extent_v<decltype(sourceNames)>)
                return sourceNames[srcIndex];
            return "";
        }

        void DumpVertexDecl(const MaterialPass& pass)
        {
            const auto* vertexDecl = pass.vertexDecl;
            if (vertexDecl == nullptr)
                return;

            if(vertexDecl->name && vertexDecl->name[0] == ',')
            {
                const auto loadedVertexDeclFromOtherZone = GlobalAssetPool<MaterialVertexDeclaration>::GetAssetByName(&vertexDecl->name[1]);

                if (loadedVertexDeclFromOtherZone == nullptr)
                {
                    // Cannot dump when shader is referenced due to unknown constant names and unknown version
                    Indent();
                    std::cerr << "Cannot dump vertex decl " << &vertexDecl->name[1] << " due to being a referenced asset\n";
                    m_stream << "// Cannot dump vertex decl " << &vertexDecl->name[1] << " due to being a referenced asset\n";
                    return;
                }
                vertexDecl = loadedVertexDeclFromOtherZone->Asset();
            }

            m_stream << "\n";

#ifdef TECHSET_DEBUG
            Indent();
            m_stream << "// Decl: " << vertexDecl->name << "\n";
#endif

            const auto streamCount = std::min(static_cast<size_t>(vertexDecl->streamCount), std::extent_v<decltype(MaterialVertexStreamRouting::data)>);
            for (auto streamIndex = 0u; streamIndex < streamCount; streamIndex++)
            {
                const auto& stream = vertexDecl->routing.data[streamIndex];
                Indent();
                m_stream << "vertex." << GetStreamDestinationString(static_cast<MaterialStreamDestination_e>(stream.dest))
                    << " = code." << GetStreamSourceString(static_cast<MaterialStreamStreamSource_e>(stream.source)) << ";\n";
            }
        }

        void DumpPass(const MaterialPass& pass)
        {
            m_stream << "{\n";
            IncIndent();

            DumpStateMap();
            DumpVertexShader(pass);
            DumpPixelShader(pass);
            DumpVertexDecl(pass);

            DecIndent();
            m_stream << "}\n";
        }

    public:
        explicit TechniqueFileWriter(std::ostream& stream)
            : AbstractTextDumper(stream)
        {
        }

        void DumpTechnique(const MaterialTechnique* technique)
        {
            for (auto i = 0u; i < technique->passCount; i++)
                DumpPass(technique->passArray[i]);
        }
    };

    class TechsetFileWriter : public AbstractTextDumper
    {
        bool m_last_write_was_value;

    public:
        explicit TechsetFileWriter(std::ostream& stream)
            : AbstractTextDumper(stream),
              m_last_write_was_value(false)
        {
        }

        void WriteTechniqueType(const size_t techniqueIndex)
        {
            assert(techniqueIndex < std::extent_v<decltype(techniqueTypeNames)>);

            if (m_last_write_was_value)
            {
                m_stream << "\n";
                m_last_write_was_value = false;
            }
            m_stream << '"' << techniqueTypeNames[techniqueIndex] << "\":\n";
        }

        void WriteTechniqueValue(const char* value)
        {
            m_last_write_was_value = true;

            IncIndent();
            Indent();
            m_stream << value << ";\n";
            DecIndent();
        }

        void DumpTechset(const MaterialTechniqueSet* techset)
        {
            std::vector<bool> dumpedTechniques(std::extent_v<decltype(MaterialTechniqueSet::techniques)>);

            for (auto techniqueIndex = 0u; techniqueIndex < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; techniqueIndex++)
            {
                const auto* technique = techset->techniques[techniqueIndex];
                if (technique == nullptr || dumpedTechniques[techniqueIndex])
                    continue;

                dumpedTechniques[techniqueIndex] = true;
                WriteTechniqueType(techniqueIndex);

                for (auto nextTechniqueIndex = techniqueIndex + 1; nextTechniqueIndex < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; nextTechniqueIndex++)
                {
                    if (techset->techniques[nextTechniqueIndex] != technique)
                        continue;

                    dumpedTechniques[nextTechniqueIndex] = true;
                    WriteTechniqueType(nextTechniqueIndex);
                }

                WriteTechniqueValue(technique->name);
            }
        }
    };
}

std::string AssetDumperTechniqueSet::GetTechniqueFileName(const MaterialTechnique* technique)
{
    std::ostringstream ss;
    ss << "techniques/" << technique->name << ".tech";
    return ss.str();
}

std::string AssetDumperTechniqueSet::GetTechsetFileName(const MaterialTechniqueSet* techset)
{
    std::ostringstream ss;
    ss << "techsets/" << techset->name << ".techset";
    return ss.str();
}

bool AssetDumperTechniqueSet::ShouldDump(XAssetInfo<MaterialTechniqueSet>* asset)
{
    return true;
}

void AssetDumperTechniqueSet::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialTechniqueSet>* asset)
{
    const auto* techset = asset->Asset();

    const auto techsetFile = context.OpenAssetFile(GetTechsetFileName(techset));

    if (techsetFile)
    {
        TechsetFileWriter writer(*techsetFile);
        writer.DumpTechset(techset);
    }

    auto* techniqueState = context.GetZoneAssetDumperState<TechniqueDumpingZoneState>();
    for (const auto* technique : techset->techniques)
    {
        if (technique && techniqueState->ShouldDumpTechnique(technique))
        {
            const auto techniqueFile = context.OpenAssetFile(GetTechniqueFileName(technique));
            if (techniqueFile)
            {
                TechniqueFileWriter writer(*techniqueFile);
                writer.DumpTechnique(technique);
            }
        }
    }
}
