#include "Image/DdsLoader.h"
#include "Image/DdsWriter.h"
#include "Image/Texture.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstring>
#include <sstream>

TEST_CASE("DDS loading distinguishes BGR pixel strides with identical channel masks", "[image][dds]")
{
    const auto* format = GENERATE(&image::format::B8_G8_R8, &image::format::B8_G8_R8_X8);
    image::Texture2D source(format, 4, 4, true);
    source.Allocate();
    for (auto mip = 0; mip < source.GetMipMapCount(); ++mip)
        for (auto i = 0uz; i < source.GetSizeOfMipLevel(mip); ++i)
            source.GetBufferForMipLevel(mip, 0)[i] = static_cast<uint8_t>(i * 13 + mip);

    std::stringstream file(std::ios::in | std::ios::out | std::ios::binary);
    image::DdsWriter writer;
    writer.DumpImage(file, &source);
    file.seekg(0);
    const auto loaded = image::LoadDds(file);
    REQUIRE(loaded);
    REQUIRE(loaded->GetFormat()->GetId() == format->GetId());
    REQUIRE(loaded->GetMipMapCount() == source.GetMipMapCount());
    for (auto mip = 0; mip < source.GetMipMapCount(); ++mip)
    {
        REQUIRE(loaded->GetSizeOfMipLevel(mip) == source.GetSizeOfMipLevel(mip));
        CHECK(std::memcmp(loaded->GetBufferForMipLevel(mip), source.GetBufferForMipLevel(mip, 0), source.GetSizeOfMipLevel(mip)) == 0);
    }
}
