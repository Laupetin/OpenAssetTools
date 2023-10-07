#include "Linker.h"

int main(const int argc, const char** argv)
{
    const auto linker = Linker::Create();

    return linker->Start(argc, argv) ? 0 : 1;
}
