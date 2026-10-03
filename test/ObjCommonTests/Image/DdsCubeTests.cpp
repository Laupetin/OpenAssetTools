#include "Image/DdsLoader.h"
#include "Image/DdsTypes.h"
#include "Image/DdsWriter.h"
#include "Image/Texture.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

namespace image
{
    namespace
    {
        void FillCube(TextureCube& texture)
        {
            for (auto face = 0; face < texture.GetFaceCount(); face++)
            {
                for (auto mip = 0; mip < texture.GetMipMapCount(); mip++)
                {
                    const auto value = static_cast<unsigned char>(face * 16 + mip + 1);
                    std::memset(texture.GetBufferForMipLevel(mip, face), value, texture.GetSizeOfMipLevel(mip));
                }
            }
        }

        std::vector<uint8_t> GetFaceMajorData(const TextureCube& texture)
        {
            std::vector<uint8_t> data;
            for (auto face = 0; face < texture.GetFaceCount(); face++)
            {
                for (auto mip = 0; mip < texture.GetMipMapCount(); mip++)
                {
                    const auto* buffer = texture.GetBufferForMipLevel(mip, face);
                    const auto mipSize = texture.GetSizeOfMipLevel(mip);
                    data.insert(data.end(), buffer, buffer + mipSize);
                }
            }

            return data;
        }

        void RequireEqualCubeTextures(const TextureCube& expected, const Texture& actual)
        {
            REQUIRE(actual.GetTextureType() == TextureType::T_CUBE);
            REQUIRE(actual.GetMipMapCount() == expected.GetMipMapCount());

            for (auto face = 0; face < expected.GetFaceCount(); face++)
            {
                for (auto mip = 0; mip < expected.GetMipMapCount(); mip++)
                {
                    const auto mipSize = expected.GetSizeOfMipLevel(mip);
                    REQUIRE(std::memcmp(actual.GetBufferForMipLevel(mip, face), expected.GetBufferForMipLevel(mip, face), mipSize) == 0);
                }
            }
        }
    } // namespace

    TEST_CASE("DdsWriter stores cubemap mip chains face-major", "[image][dds]")
    {
        TextureCube source(&format::A8, 8, 8, true);
        source.Allocate();
        FillCube(source);

        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        DdsWriter writer;
        writer.DumpImage(stream, &source);

        const auto data = stream.str();
        const auto expectedData = GetFaceMajorData(source);
        constexpr auto dataOffset = sizeof(uint32_t) + sizeof(DDS_HEADER);

        REQUIRE(data.size() == dataOffset + expectedData.size());
        REQUIRE(std::memcmp(data.data() + dataOffset, expectedData.data(), expectedData.size()) == 0);
    }

    TEST_CASE("DdsLoader reads face-major cubemap mip chains", "[image][dds]")
    {
        TextureCube source(&format::A8, 8, 8, true);
        source.Allocate();
        FillCube(source);

        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        DdsWriter writer;
        writer.DumpImage(stream, &source);
        stream.seekg(0);

        const auto loaded = LoadDds(stream);
        REQUIRE(loaded);
        RequireEqualCubeTextures(source, *loaded);
    }
} // namespace image
