#include "Image/Dx9TextureLoader.h"
#include "Image/Texture.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
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

    TEST_CASE("Dx9TextureLoader reads face-major cubemap mip chains", "[image][dx9]")
    {
        TextureCube source(&format::A8, 8, 8, true);
        source.Allocate();
        FillCube(source);

        const auto data = GetFaceMajorData(source);
        Dx9TextureLoader loader;
        const auto loaded = loader.Format(source.GetFormat()->GetD3DFormat())
                                .Type(TextureType::T_CUBE)
                                .HasMipMaps(true)
                                .CubeMapOrder(Dx9TextureLoader::CubeMapDataOrder::FaceMajor)
                                .Width(source.GetWidth())
                                .Height(source.GetHeight())
                                .LoadTexture(data.data());

        REQUIRE(loaded);
        RequireEqualCubeTextures(source, *loaded);
    }
} // namespace image
