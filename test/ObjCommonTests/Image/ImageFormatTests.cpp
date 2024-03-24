#include "Image/ImageFormat.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <type_traits>

namespace image::image_format
{
    TEST_CASE("ImageFormat: EnsureAllFormatsArrayIndicesAreIds", "[image]")
    {
        REQUIRE(static_cast<unsigned int>(ImageFormatId::MAX) == std::extent_v<decltype(ImageFormat::ALL_FORMATS)>);

        for (unsigned i = 0; i < std::extent_v<decltype(ImageFormat::ALL_FORMATS)>; i++)
        {
            REQUIRE(ImageFormat::ALL_FORMATS[i] != nullptr);
            REQUIRE(i == static_cast<unsigned>(ImageFormat::ALL_FORMATS[i]->GetId()));
        }
    }
} // namespace image::image_format
