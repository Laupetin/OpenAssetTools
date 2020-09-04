#include "CppUnitTest.h"
#include "Image/ImageFormat.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ObjCommonTests
{
	TEST_CLASS(ImageFormatTests)
	{
	public:
		TEST_METHOD(EnsureAllFormatsArrayIndicesAreIds)
		{
			Assert::AreEqual(static_cast<unsigned int>(ImageFormatId::MAX), _countof(ImageFormat::ALL_FORMATS));

			for(unsigned i = 0; i < _countof(ImageFormat::ALL_FORMATS); i++)
			{
				Assert::IsNotNull(ImageFormat::ALL_FORMATS[i]);
				Assert::AreEqual(i, static_cast<unsigned>(ImageFormat::ALL_FORMATS[i]->GetId()));
			}
		}
	};
}