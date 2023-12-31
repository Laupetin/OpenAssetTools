#pragma once

#include "AssetLoading/IZoneAssetLoaderState.h"

#include <string>
#include <unordered_set>

class LocalizeReadingZoneState final : public IZoneAssetLoaderState
{
public:
    /**
     * Checks whether a localize key was already added.
     * Inserts key if it was not added yet.
     *
     * \param key The key to check
     * \returns \c true if key was not duplicated yet, \c false otherwise
     */
    bool DoLocalizeEntryDuplicateCheck(const std::string& key);

private:
    std::unordered_set<std::string> m_keys;
};
