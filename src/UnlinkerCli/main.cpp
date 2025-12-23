#include "Unlinker.h"
#include "Utils/Logging/Log.h"

int main(const int argc, const char** argv)
{
    con::init();

    UnlinkerArgs args;
    auto shouldContinue = true;
    if (!args.ParseArgs(argc, argv, shouldContinue))
        return 1;

    if (!shouldContinue)
        return 0;

    const auto unlinker = Unlinker::Create(std::move(args));

    return unlinker->Start() ? 0 : 1;
}
