#include "Linker.h"
#include "Utils/Logging/Log.h"

int main(const int argc, const char** argv)
{
    con::init();

    LinkerArgs args;
    auto shouldContinue = true;
    if (!args.ParseArgs(argc, argv, shouldContinue))
        return 1;

    if (!shouldContinue)
        return 0;

    const auto linker = Linker::Create(std::move(args));

    return linker->Start() ? 0 : 1;
}
