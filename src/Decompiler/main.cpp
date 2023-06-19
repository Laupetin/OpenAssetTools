#include "Decompiler.h"

int main(const int argc, const char** argv)
{
    const auto decompiler = Decompiler::Create();

    return decompiler->Start(argc, argv) ? 0 : 1;
}
