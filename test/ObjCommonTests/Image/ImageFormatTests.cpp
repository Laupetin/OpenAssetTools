#include "Image/ImageFormat.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <type_traits>

namespace image::image_format
{
    TEST_CASE("ImageFormat: EnsureAllFormatsArrayIndicesAreIds", "[image]")
    {
        REQUIRE(static_cast<unsigned int>(ImageFormatId::MAX) == std::extent_v<decltype(format::ALL)>);

        for (unsigned i = 0; i < std::extent_v<decltype(format::ALL)>; i++)
        {
            REQUIRE(format::ALL[i] != nullptr);
            REQUIRE(i == static_cast<unsigned>(format::ALL[i]->GetId()));
        }
    }
} // namespace image::image_format
