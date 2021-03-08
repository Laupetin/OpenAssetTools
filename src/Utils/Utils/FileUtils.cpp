#include "FileUtils.h"

#include <sstream>

bool FileUtils::ParsePathsString(const std::string& pathsString, std::set<std::string>& output)
{
    std::ostringstream currentPath;
    auto pathStart = true;
    auto quotationPos = 0; // 0 = before quotations, 1 = in quotations, 2 = after quotations

    for (auto character : pathsString)
    {
        switch (character)
        {
        case '"':
            if (quotationPos == 0 && pathStart)
            {
                quotationPos = 1;
            }
            else if (quotationPos == 1)
            {
                quotationPos = 2;
                pathStart = false;
            }
            else
            {
                return false;
            }
            break;

        case ';':
            if (quotationPos != 1)
            {
                auto path = currentPath.str();
                if (!path.empty())
                {
                    output.insert(path);
                    currentPath = std::ostringstream();
                }

                pathStart = true;
                quotationPos = 0;
            }
            else
            {
                currentPath << character;
            }
            break;

        default:
            currentPath << character;
            pathStart = false;
            break;
        }
    }

    if (currentPath.tellp() > 0)
    {
        output.insert(currentPath.str());
    }

    return true;
}
