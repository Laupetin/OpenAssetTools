#include "Unlinker.h"

int main(const int argc, const char** argv)
{
    const auto unlinker = Unlinker::Create();

    return unlinker->Start(argc, argv) ? 0 : 1;
}
