#pragma once

#ifdef _WIN32
#include "Windows/DialogHandlerWindows.h"
#elif defined(__linux__)
#include "Linux/DialogHandlerLinux.h"
#endif
