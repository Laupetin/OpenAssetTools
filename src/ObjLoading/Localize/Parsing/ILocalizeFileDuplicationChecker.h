#pragma once

#include <string>

class ILocalizeFileDuplicationChecker
{
public:
    virtual ~ILocalizeFileDuplicationChecker() = default;

    /**
     * Checks whether a localize key was already added.
     * Inserts key if it was not added yet.
     *
     * \param key The key to check
     * \returns \c true if key was not duplicated yet, \c false otherwise
     */
    virtual bool CheckLocalizeEntryForDuplicates(const std::string& key) = 0;
};
