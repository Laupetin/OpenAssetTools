#pragma once

#ifdef _WIN32
#include "Windows/AssetHandlerWindows.h"
#elif defined(__linux__)
#include "Linux/AssetHandlerLinux.h"
#endif
