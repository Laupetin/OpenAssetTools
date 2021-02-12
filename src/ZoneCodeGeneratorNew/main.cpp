#include <utility>
#include <vector>

#include "ZoneCodeGenerator.h"

int main(const int argc, const char** argv)
{
    int i = 5;
    std::vector<std::pair<int, const int&>> asdf;
    asdf.emplace_back(1, i);

    const ZoneCodeGenerator zoneCodeGenerator;
    return zoneCodeGenerator.Run(argc, argv);
}
