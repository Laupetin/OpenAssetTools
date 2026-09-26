#include "D3D9ShaderAnalyser.h"

#include "Utils/FileUtils.h"

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

    bool CanRead(const size_t offset, const size_t readSize, const size_t bufferSize)
    {
        return offset <= bufferSize && readSize <= bufferSize - offset;
    }

    bool ReadUint32(const uint8_t* data, const size_t dataSize, const size_t offset, uint32_t& value)
    {
        if (!CanRead(offset, sizeof(value), dataSize))
            return false;

        std::memcpy(&value, data + offset, sizeof(value));
        return true;
    }

    template<typename T> bool ReadStruct(const char* data, const size_t dataSize, const size_t offset, T& value)
    {
        if (!CanRead(offset, sizeof(T), dataSize))
            return false;

        std::memcpy(&value, data + offset, sizeof(T));
        return true;
    }

    bool ReadString(const char* data, const size_t dataSize, const size_t offset, std::string& value)
    {
        if (offset >= dataSize)
            return false;

        const auto remainingSize = dataSize - offset;
        const auto* terminator = static_cast<const char*>(std::memchr(data + offset, '\0', remainingSize));
        if (!terminator)
            return false;

        value.assign(data + offset, terminator);
        return true;
    }

    bool PopulateVersionInfo(ShaderInfo& shaderInfo, const void* shaderByteCode, const size_t shaderByteCodeSize)
    {
        if (shaderByteCodeSize < sizeof(uint32_t))
            return false;

        uint32_t version;
        std::memcpy(&version, shaderByteCode, sizeof(version));
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
        auto currentOffset = sizeof(uint32_t);
        while (CanRead(currentOffset, sizeof(uint32_t), shaderByteCodeSize))
        {
            uint32_t currentValue;
            if (!ReadUint32(shaderByteCode, shaderByteCodeSize, currentOffset, currentValue))
                return false;
            if (currentValue == OPCODE_END)
                return false;

            if ((currentValue & OPCODE_MASK) == OPCODE_COMMENT)
            {
                const auto currentCommentSize = (currentValue & COMMENT_SIZE_MASK) >> COMMENT_SIZE_SHIFT;
                if (currentCommentSize == 0 || currentCommentSize > (shaderByteCodeSize - currentOffset - sizeof(uint32_t)) / sizeof(uint32_t))
                    return false;

                uint32_t currentMagic;
                if (!ReadUint32(shaderByteCode, shaderByteCodeSize, currentOffset + sizeof(uint32_t), currentMagic))
                    return false;

                if (currentMagic == magic)
                {
                    commentStart = reinterpret_cast<const char*>(shaderByteCode + currentOffset + 2u * sizeof(uint32_t));
                    commentSize = (currentCommentSize - 1) * sizeof(uint32_t);
                    return true;
                }

                currentOffset += (static_cast<size_t>(currentCommentSize) + 1u) * sizeof(uint32_t);
                continue;
            }

            currentOffset += sizeof(uint32_t);
        }

        return false;
    }

    bool PopulateShaderConstantFromConstantInfo(ShaderConstant& shaderConstant,
                                                const char* commentStart,
                                                const size_t commentSize,
                                                const ConstantInfo& constantInfo)
    {
        if (constantInfo.Name)
        {
            if (!ReadString(commentStart, commentSize, constantInfo.Name, shaderConstant.m_name))
                return false;
        }

        shaderConstant.m_register_set = static_cast<RegisterSet>(constantInfo.RegisterSet);
        if (shaderConstant.m_register_set >= RegisterSet::MAX)
            return false;

        shaderConstant.m_register_index = constantInfo.RegisterIndex;
        shaderConstant.m_register_count = constantInfo.RegisterCount;

        if (constantInfo.TypeInfo)
        {
            TypeInfo typeInfo;
            if (!ReadStruct(commentStart, commentSize, constantInfo.TypeInfo, typeInfo))
                return false;

            shaderConstant.m_class = static_cast<ParameterClass>(typeInfo.Class);
            if (shaderConstant.m_class >= ParameterClass::MAX)
                return false;

            shaderConstant.m_type = static_cast<ParameterType>(typeInfo.Type);
            if (shaderConstant.m_type >= ParameterType::MAX)
                return false;

            shaderConstant.m_type_rows = typeInfo.Rows;
            shaderConstant.m_type_columns = typeInfo.Columns;
            shaderConstant.m_type_elements = typeInfo.Elements;
        }

        return true;
    }

    bool PopulateShaderInfoFromConstantTable(ShaderInfo& shaderInfo, const char* commentStart, const size_t commentSize, const ConstantTable& constantTable)
    {
        if (constantTable.Size != sizeof(ConstantTable))
            return false;

        if (constantTable.Creator)
        {
            if (!ReadString(commentStart, commentSize, constantTable.Creator, shaderInfo.m_creator))
                return false;
        }

        if (constantTable.Target)
        {
            if (!ReadString(commentStart, commentSize, constantTable.Target, shaderInfo.m_target))
                return false;
        }

        if (constantTable.Constants > 0 && constantTable.ConstantInfo)
        {
            if (constantTable.ConstantInfo > commentSize || constantTable.Constants > (commentSize - constantTable.ConstantInfo) / sizeof(ConstantInfo))
                return false;

            for (auto constantInfoIndex = 0u; constantInfoIndex < constantTable.Constants; constantInfoIndex++)
            {
                ConstantInfo constantInfo;
                const auto offset = static_cast<size_t>(constantTable.ConstantInfo) + static_cast<size_t>(constantInfoIndex) * sizeof(ConstantInfo);
                if (!ReadStruct(commentStart, commentSize, offset, constantInfo))
                    return false;

                ShaderConstant constant;
                if (!PopulateShaderConstantFromConstantInfo(constant, commentStart, commentSize, constantInfo))
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
        if (!FindComment(shaderByteCode, shaderByteCodeSize, utils::MakeMagic32('C', 'T', 'A', 'B'), constantTableComment, constantTableCommentSize))
            return false;

        if (constantTableCommentSize < sizeof(ConstantTable))
            return false;

        ConstantTable constantTable;
        if (!ReadStruct(constantTableComment, constantTableCommentSize, 0, constantTable)
            || !PopulateShaderInfoFromConstantTable(shaderInfo, constantTableComment, constantTableCommentSize, constantTable))
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
