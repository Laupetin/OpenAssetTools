#include "ImageConverter.h"

int main(const int argc, const char** argv)
{
    const auto imageConverter = ImageConverter::Create();

    return imageConverter->Start(argc, argv) ? 0 : 1;
}
