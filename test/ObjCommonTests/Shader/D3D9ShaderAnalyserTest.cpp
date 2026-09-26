#include "Shader/D3D9ShaderAnalyser.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace
{
    constexpr uint32_t VERTEX_SHADER_3_0 = 0xFFFE0300;
    constexpr uint32_t COMMENT_OPCODE = 0xFFFE;
    constexpr uint32_t END_OPCODE = 0xFFFF;
    constexpr uint32_t CTAB_MAGIC =
        static_cast<uint32_t>('C') | (static_cast<uint32_t>('T') << 8u) | (static_cast<uint32_t>('A') << 16u) | (static_cast<uint32_t>('B') << 24u);

    void AppendUint32(std::vector<std::byte>& data, const uint32_t value)
    {
        const auto oldSize = data.size();
        data.resize(oldSize + sizeof(value));
        std::memcpy(data.data() + oldSize, &value, sizeof(value));
    }

    std::vector<std::byte> MakeMinimalShader()
    {
        std::vector<std::byte> result;
        AppendUint32(result, VERTEX_SHADER_3_0);
        AppendUint32(result, (8u << 16u) | COMMENT_OPCODE);
        AppendUint32(result, CTAB_MAGIC);
        AppendUint32(result, 28u); // ConstantTable::Size
        AppendUint32(result, 0u);  // Creator
        AppendUint32(result, VERTEX_SHADER_3_0);
        AppendUint32(result, 0u); // Constants
        AppendUint32(result, 0u); // ConstantInfo
        AppendUint32(result, 0u); // Flags
        AppendUint32(result, 0u); // Target
        AppendUint32(result, END_OPCODE);
        return result;
    }
} // namespace

TEST_CASE("D3D9ShaderAnalyser accepts a bounded constant table", "[shader][d3d9]")
{
    const auto shader = MakeMinimalShader();
    const auto info = d3d9::ShaderAnalyser::GetShaderInfo(shader.data(), shader.size());

    REQUIRE(info);
    REQUIRE(info->m_type == d3d9::ShaderType::VERTEX_SHADER);
    REQUIRE(info->m_version_major == 3u);
    REQUIRE(info->m_version_minor == 0u);
    REQUIRE(info->m_constants.empty());
}

TEST_CASE("D3D9ShaderAnalyser rejects truncated instructions and comments", "[shader][d3d9]")
{
    SECTION("Version token without instructions")
    {
        const std::array shader{VERTEX_SHADER_3_0};
        REQUIRE_FALSE(d3d9::ShaderAnalyser::GetShaderInfo(shader.data(), sizeof(shader)));
    }

    SECTION("Comment without magic token")
    {
        const std::array shader{VERTEX_SHADER_3_0, (1u << 16u) | COMMENT_OPCODE};
        REQUIRE_FALSE(d3d9::ShaderAnalyser::GetShaderInfo(shader.data(), sizeof(shader)));
    }

    SECTION("Zero-sized comment")
    {
        const std::array shader{VERTEX_SHADER_3_0, COMMENT_OPCODE, CTAB_MAGIC, END_OPCODE};
        REQUIRE_FALSE(d3d9::ShaderAnalyser::GetShaderInfo(shader.data(), sizeof(shader)));
    }

    SECTION("Comment extends past the buffer")
    {
        const std::array shader{VERTEX_SHADER_3_0, (0xFFFFu << 16u) | COMMENT_OPCODE, CTAB_MAGIC, END_OPCODE};
        REQUIRE_FALSE(d3d9::ShaderAnalyser::GetShaderInfo(shader.data(), sizeof(shader)));
    }
}

TEST_CASE("D3D9ShaderAnalyser rejects invalid constant-table offsets", "[shader][d3d9]")
{
    SECTION("Creator starts at the end of the comment")
    {
        auto shader = MakeMinimalShader();
        constexpr uint32_t invalidCreatorOffset = 28u;
        std::memcpy(shader.data() + 4u * sizeof(uint32_t), &invalidCreatorOffset, sizeof(invalidCreatorOffset));
        REQUIRE_FALSE(d3d9::ShaderAnalyser::GetShaderInfo(shader.data(), shader.size()));
    }

    SECTION("Constant array extends past the comment")
    {
        auto shader = MakeMinimalShader();
        constexpr uint32_t constantCount = 0xFFFFFFFFu;
        constexpr uint32_t constantInfoOffset = 1u;
        std::memcpy(shader.data() + 6u * sizeof(uint32_t), &constantCount, sizeof(constantCount));
        std::memcpy(shader.data() + 7u * sizeof(uint32_t), &constantInfoOffset, sizeof(constantInfoOffset));
        REQUIRE_FALSE(d3d9::ShaderAnalyser::GetShaderInfo(shader.data(), shader.size()));
    }
}

TEST_CASE("D3D9ShaderAnalyser safely handles truncated and malformed bytecode", "[shader][d3d9]")
{
    SECTION("Every truncated prefix at an unaligned address")
    {
        const auto shader = MakeMinimalShader();
        std::vector<std::byte> unalignedShader(shader.size() + 1u);
        std::memcpy(unalignedShader.data() + 1u, shader.data(), shader.size());

        for (size_t size = 0; size <= shader.size(); size++)
            REQUIRE_NOTHROW(d3d9::ShaderAnalyser::GetShaderInfo(unalignedShader.data() + 1u, size));
    }

    SECTION("Deterministic malformed-input corpus")
    {
        uint32_t state = 0xC0FFEEu;
        for (size_t size = 1; size <= 256u; size++)
        {
            std::vector<std::byte> data(size);
            for (auto& value : data)
            {
                state = state * 1664525u + 1013904223u;
                value = static_cast<std::byte>(state >> 24u);
            }

            if (size >= sizeof(VERTEX_SHADER_3_0))
                std::memcpy(data.data(), &VERTEX_SHADER_3_0, sizeof(VERTEX_SHADER_3_0));

            REQUIRE_NOTHROW(d3d9::ShaderAnalyser::GetShaderInfo(data.data(), data.size()));
        }
    }
}
