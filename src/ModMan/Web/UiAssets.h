#pragma once

#include "ui/modmanui.h"

#include <string>
#include <unordered_map>

std::unordered_map<std::string, UiFile> BuildUiFileLookup();
const char* GetMimeTypeForFileName(const std::string& fileName);
