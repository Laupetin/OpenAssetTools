#include "AutoSearchPathsT6.h"

const std::vector<std::string>& AutoSearchPathsT6::RecognizedZoneDirs() const
{
    static std::vector<std::string> recognizedZoneDirs = {
        "zone/all",
        "zone/english",
        "zone/french",
        "zone/frenchcan",
        "zone/german",
        "zone/austrian",
        "zone/italian",
        "zone/spanish",
        "zone/british",
        "zone/russian",
        "zone/polish",
        "zone/korean",
        "zone/japanese",
        "zone/czech",
        "zone/fulljap",
        "zone/portuguese",
        "zone/mexicanspanish",
    };
    return recognizedZoneDirs;
}

const std::vector<std::string>& AutoSearchPathsT6::AdditionalSearchPaths() const
{
    static std::vector<std::string> additionalSearchPaths = {
        "main",
        "sound",
    };
    return additionalSearchPaths;
}
