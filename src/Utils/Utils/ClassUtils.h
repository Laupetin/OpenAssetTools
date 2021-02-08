#pragma once

#define STR(x) #x

#if defined(_MSVC_LANG)
#define _CPP_VERSION _MSVC_LANG
#elif defined(__cplusplus)
#define _CPP_VERSION __cplusplus
#else
#define _CPP_VERSION 0
#endif

#ifndef _NODISCARD
#if _CPP_VERSION >= 201703L
#define _NODISCARD [[nodiscard]]
#else
#define _NODISCARD
#endif
#endif