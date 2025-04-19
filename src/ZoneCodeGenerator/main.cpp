#include "ZoneCodeGenerator.h"

int main(const int argc, const char** argv)
{
    auto zoneCodeGenerator = ZoneCodeGenerator::Create();
    return zoneCodeGenerator->Run(argc, argv);
}
