#include "AutoSearchPathsT4.h"

const std::vector<std::string>& AutoSearchPathsT4::RecognizedZoneDirs() const
{
    static std::vector<std::string> recognizedZoneDirs = {
        "zone/English",
        "zone/French",
        "zone/German",
        "zone/Italian",
        "zone/Spanish",
        "zone/British",
        "zone/Russian",
        "zone/Polish",
        "zone/Korean",
        "zone/Taiwanese",
        "zone/Japanese",
        "zone/Chinese",
        "zone/Thai",
        "zone/Leet",
        "zone/Czech",
    };
    return recognizedZoneDirs;
}

const std::vector<std::string>& AutoSearchPathsT4::AdditionalSearchPaths() const
{
    static std::vector<std::string> additionalSearchPaths = {
        "main",
    };
    return additionalSearchPaths;
}
