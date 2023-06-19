#include "Compiler.h"

#include "CompilerArgs.h"

class CompilerImpl final : public Compiler
{
    CompilerArgs m_args;

public:
    bool Start(const int argc, const char** argv) override
    {
        if (!m_args.ParseArgs(argc, argv))
            return false;

        return true;
    }
};

std::unique_ptr<Compiler> Compiler::Create()
{
    return std::make_unique<CompilerImpl>();
}
