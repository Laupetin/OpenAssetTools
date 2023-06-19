#include "Compiler.h"

int main(const int argc, const char** argv)
{
    const auto compiler = Compiler::Create();

    return compiler->Start(argc, argv) ? 0 : 1;
}
