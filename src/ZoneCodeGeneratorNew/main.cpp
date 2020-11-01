#include <cstdio>
#include <string>

#include "Interface/Session.h"
#include "Interface/ZoneCodeGeneratorArguments.h"

int main(const int argc, const char** argv)
{
    ZoneCodeGeneratorArguments args;

    if(!args.Parse(argc, argv))
        return 1;

    Session session(args);

    const std::string asdf = "Hello World";
    printf("%s\n", asdf.c_str());
}
