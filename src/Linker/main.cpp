#include "Linker.h"

int main(const int argc, const char** argv)
{
    Linker linker;

    return linker.Start(argc, argv) ? 0 : 1;
}
