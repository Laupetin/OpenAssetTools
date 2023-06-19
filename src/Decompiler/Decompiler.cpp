#include "Decompiler.h"

#include "DecompilerArgs.h"

class DecompilerImpl final : public Decompiler
{
    DecompilerArgs m_args;

public:
    bool Start(const int argc, const char** argv) override
    {
        if (!m_args.ParseArgs(argc, argv))
            return false;

        return true;
    }
};

std::unique_ptr<Decompiler> Decompiler::Create()
{
    return std::make_unique<DecompilerImpl>();
}
