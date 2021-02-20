#include <catch2/catch.hpp>

#include "Image/ImageFormat.h"

namespace image::image_format
{
	TEST_CASE("ImageFormat: EnsureAllFormatsArrayIndicesAreIds", "[image]")
	{
		REQUIRE(static_cast<unsigned int>(ImageFormatId::MAX) == _countof(ImageFormat::ALL_FORMATS));

		for(unsigned i = 0; i < _countof(ImageFormat::ALL_FORMATS); i++)
		{
			REQUIRE(ImageFormat::ALL_FORMATS[i] != nullptr);
			REQUIRE(i == static_cast<unsigned>(ImageFormat::ALL_FORMATS[i]->GetId()));
		}
	}
}