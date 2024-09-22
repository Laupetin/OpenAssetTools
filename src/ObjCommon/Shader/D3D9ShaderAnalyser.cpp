#include "D3D9ShaderAnalyser.h"

#include "Utils/FileUtils.h"

#include <cassert>
#include <cstring>

using namespace d3d9;

namespace d3d9
{
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/d3d9types/ne-d3d9types-_d3dshader_instruction_opcode_type
    static constexpr uint32_t OPCODE_COMMENT = 0xFFFE;
    static constexpr uint32_t OPCODE_END = 0xFFFF;

    static constexpr uint32_t OPCODE_MASK = 0x0000FFFF;

    static constexpr uint32_t COMMENT_SIZE_MASK = 0xFFFF0000;
    static constexpr uint32_t COMMENT_SIZE_SHIFT = 16;

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxshader-constanttable
    struct ConstantTable
    {
        uint32_t Size;
        uint32_t Creator;
        uint32_t Version;
        uint32_t Constants;
        uint32_t ConstantInfo;
        uint32_t Flags;
        uint32_t Target;
    };

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxshader-constantinfo
    struct ConstantInfo
    {
        uint32_t Name;
        uint16_t RegisterSet;
        uint16_t RegisterIndex;
        uint16_t RegisterCount;
        uint16_t Reserved;
        uint32_t TypeInfo;
        uint32_t DefaultValue;
    };

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxshader-typeinfo
    struct TypeInfo
    {
        uint16_t Class;
        uint16_t Type;
        uint16_t Rows;
        uint16_t Columns;
        uint16_t Elements;
        uint16_t StructMembers;
        uint32_t StructMemberInfo;
    };

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxshader-structmemberinfo
    struct StructMemberInfo
    {
        uint32_t Name;
        uint32_t TypeInfo;
    };

    bool PopulateVersionInfo(ShaderInfo& shaderInfo, const void* shaderByteCode, const size_t shaderByteCodeSize)
    {
        if (shaderByteCodeSize < sizeof(uint32_t))
            return false;

        const auto version = *static_cast<const uint32_t*>(shaderByteCode);
        shaderInfo.m_version_minor = version & 0xFF;
        shaderInfo.m_version_major = (version & 0xFF00) >> 8;

        switch ((version & 0xFFFF0000) >> 16)
        {
        case 0x4658:                                 // FX
        case 0x5458:                                 // TX
        case 0x7ffe:                                 // ?
        case 0x7fff:                                 // ?
            shaderInfo.m_type = ShaderType::UNKNOWN; // Valid according to wine
            return true;
        case 0xfffe:
            shaderInfo.m_type = ShaderType::VERTEX_SHADER;
            return true;
        case 0xffff:
            shaderInfo.m_type = ShaderType::PIXEL_SHADER;
            return true;
        default:
            break;
        }

        return false;
    }

    bool FindComment(const uint8_t* shaderByteCode, const size_t shaderByteCodeSize, const uint32_t magic, const char*& commentStart, size_t& commentSize)
    {
        const auto* currentPos = reinterpret_cast<const uint32_t*>(shaderByteCode + sizeof(uint32_t));
        auto currentOffset = sizeof(uint32_t);
        while (*currentPos != OPCODE_END && (currentOffset + sizeof(uint32_t) - 1) < shaderByteCodeSize)
        {
            const auto currentValue = *currentPos;
            if ((currentValue & OPCODE_MASK) == OPCODE_COMMENT)
            {
                assert(currentOffset + sizeof(uint32_t) < shaderByteCodeSize);
                if (currentOffset + sizeof(uint32_t) >= shaderByteCodeSize)
                    return false;

                const auto currentCommentSize = (currentValue & COMMENT_SIZE_MASK) >> COMMENT_SIZE_SHIFT;

                if (currentPos[1] == magic)
                {
                    commentStart = reinterpret_cast<const char*>(currentPos + 2);
                    commentSize = (currentCommentSize - 1) * sizeof(uint32_t);
                    return currentOffset + sizeof(uint32_t) * (currentCommentSize + 1) <= shaderByteCodeSize;
                }

                currentPos += currentCommentSize;
                currentOffset += currentCommentSize * sizeof(uint32_t);
            }

            currentPos++;
            currentOffset += sizeof(uint32_t);
            assert((currentOffset + sizeof(uint32_t) - 1) < shaderByteCodeSize);
        }

        return false;
    }

    bool StringFitsInComment(const char* str, const char* commentStart, const size_t commentSize)
    {
        const auto strLen = strnlen(str, commentSize - (str - commentStart));
        return str[strLen] == '\0';
    }

    bool PopulateShaderConstantFromConstantInfo(ShaderConstant& shaderConstant,
                                                const char* commentStart,
                                                const size_t commentSize,
                                                const ConstantInfo& constantInfo)
    {
        if (constantInfo.Name)
        {
            const auto* constantName = commentStart + constantInfo.Name;
            if (!StringFitsInComment(constantName, commentStart, commentSize))
                return false;
            shaderConstant.m_name = std::string(constantName);
        }

        shaderConstant.m_register_set = static_cast<RegisterSet>(constantInfo.RegisterSet);
        if (shaderConstant.m_register_set >= RegisterSet::MAX)
            return false;

        shaderConstant.m_register_index = constantInfo.RegisterIndex;
        shaderConstant.m_register_count = constantInfo.RegisterCount;

        if (constantInfo.TypeInfo)
        {
            assert(commentStart + constantInfo.TypeInfo + sizeof(TypeInfo) <= commentStart + commentSize);
            if (commentStart + constantInfo.TypeInfo + sizeof(TypeInfo) > commentStart + commentSize)
                return false;

            const auto* typeInfo = reinterpret_cast<const TypeInfo*>(commentStart + constantInfo.TypeInfo);

            shaderConstant.m_class = static_cast<ParameterClass>(typeInfo->Class);
            if (shaderConstant.m_class >= ParameterClass::MAX)
                return false;

            shaderConstant.m_type = static_cast<ParameterType>(typeInfo->Type);
            if (shaderConstant.m_type >= ParameterType::MAX)
                return false;

            shaderConstant.m_type_rows = typeInfo->Rows;
            shaderConstant.m_type_columns = typeInfo->Columns;
            shaderConstant.m_type_elements = typeInfo->Elements;
        }

        return true;
    }

    bool PopulateShaderInfoFromConstantTable(ShaderInfo& shaderInfo, const char* commentStart, const size_t commentSize, const ConstantTable& constantTable)
    {
        if (constantTable.Size != sizeof(ConstantTable))
            return false;

        if (constantTable.Creator)
        {
            const auto* creatorName = commentStart + constantTable.Creator;
            if (!StringFitsInComment(creatorName, commentStart, commentSize))
                return false;
            shaderInfo.m_creator = std::string(creatorName);
        }

        if (constantTable.Target)
        {
            const auto* targetName = commentStart + constantTable.Target;
            if (!StringFitsInComment(targetName, commentStart, commentSize))
                return false;
            shaderInfo.m_target = std::string(targetName);
        }

        if (constantTable.Constants > 0 && constantTable.ConstantInfo)
        {
            assert(commentStart + constantTable.ConstantInfo + sizeof(ConstantInfo) * constantTable.Constants <= commentStart + commentSize);
            if (commentStart + constantTable.ConstantInfo + sizeof(ConstantInfo) * constantTable.Constants > commentStart + commentSize)
                return false;

            const auto* constantInfos = reinterpret_cast<const ConstantInfo*>(commentStart + constantTable.ConstantInfo);
            for (auto constantInfoIndex = 0u; constantInfoIndex < constantTable.Constants; constantInfoIndex++)
            {
                ShaderConstant constant;
                if (!PopulateShaderConstantFromConstantInfo(constant, commentStart, commentSize, constantInfos[constantInfoIndex]))
                    return false;
                shaderInfo.m_constants.emplace_back(std::move(constant));
            }
        }

        return true;
    }

    bool PopulateShaderInfoFromShaderByteCode(ShaderInfo& shaderInfo, const uint8_t* shaderByteCode, const size_t shaderByteCodeSize)
    {
        if (!PopulateVersionInfo(shaderInfo, shaderByteCode, shaderByteCodeSize))
            return false;

        const char* constantTableComment;
        size_t constantTableCommentSize;
        if (!FindComment(shaderByteCode, shaderByteCodeSize, FileUtils::MakeMagic32('C', 'T', 'A', 'B'), constantTableComment, constantTableCommentSize))
            return false;

        if (constantTableCommentSize < sizeof(ConstantTable))
            return false;

        const auto* constantTable = reinterpret_cast<const ConstantTable*>(constantTableComment);
        if (!PopulateShaderInfoFromConstantTable(shaderInfo, constantTableComment, constantTableCommentSize, *constantTable))
            return false;

        return true;
    }
} // namespace d3d9

std::unique_ptr<ShaderInfo> ShaderAnalyser::GetShaderInfo(const void* shaderByteCode, const size_t shaderByteCodeSize)
{
    if (shaderByteCode == nullptr || shaderByteCodeSize == 0)
        return nullptr;

    auto shaderInfo = std::make_unique<ShaderInfo>();

    if (!PopulateShaderInfoFromShaderByteCode(*shaderInfo, static_cast<const uint8_t*>(shaderByteCode), shaderByteCodeSize))
        return nullptr;

    return shaderInfo;
}
