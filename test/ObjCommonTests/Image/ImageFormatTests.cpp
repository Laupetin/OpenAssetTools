#include <catch2/catch.hpp>

#include <type_traits>

#include "Image/ImageFormat.h"

namespace image::image_format
{
	TEST_CASE("ImageFormat: EnsureAllFormatsArrayIndicesAreIds", "[image]")
	{
		REQUIRE(static_cast<unsigned int>(ImageFormatId::MAX) == std::extent<decltype(ImageFormat::ALL_FORMATS)>::value);

		for(unsigned i = 0; i < std::extent<decltype(ImageFormat::ALL_FORMATS)>::value; i++)
		{
			REQUIRE(ImageFormat::ALL_FORMATS[i] != nullptr);
			REQUIRE(i == static_cast<unsigned>(ImageFormat::ALL_FORMATS[i]->GetId()));
		}
	}
}