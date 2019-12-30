#include "Unlinker.h"

int main(const int argc, const char** argv)
{
    Unlinker unlinker;

    return unlinker.Start(argc, argv) ? 0 : 1;
}
