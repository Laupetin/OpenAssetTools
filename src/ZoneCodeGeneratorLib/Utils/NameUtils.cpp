#include "NameUtils.h"

#include <random>
#include <sstream>

std::string NameUtils::GenerateRandomName()
{
    static constexpr auto ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::ostringstream str;

    std::random_device random;

    for (auto i = 0u; i < 32u; i++)
        str << ALPHABET[random() % std::char_traits<char>::length(ALPHABET)];

    return str.str();
}
