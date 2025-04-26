#pragma once

// Undefine any previously defined utilities
#ifdef type_align
#undef type_align
#endif
#ifdef tdef_align
#undef tdef_align
#endif
#ifdef gcc_align
#undef gcc_align
#endif

#ifdef __zonecodegenerator
#define type_align(x) alignas(x)
#define tdef_align(x) alignas(x)
#define gcc_align(x)
#else
#ifdef __ida
#define type_align(x) __declspec(align(x))
#define tdef_align(x) __declspec(align(x))
#define gcc_align(x)
#else
#ifdef _MSVC_LANG
#define type_align(x) __declspec(align(x))
#define tdef_align(x) __declspec(align(x))
#define gcc_align(x)
#else
#define type_align(x) __attribute__((__aligned__(x)))
#define tdef_align(x)
#define gcc_align(x) __attribute__((__aligned__(x)))
#endif
#endif
#endif

#if defined(__zonecodegenerator) || defined(__ida)
#define type_align32(x) type_align(x)
#define tdef_align32(x) tdef_align(x)
#define gcc_align32(x) gcc_align(x)
#define type_align64(x) type_align(x)
#define tdef_align64(x) tdef_align(x)
#define gcc_align64(x) gcc_align(x)
#elif defined(ARCH_x86)
#define type_align32(x) type_align(x)
#define tdef_align32(x) tdef_align(x)
#define gcc_align32(x) gcc_align(x)
#define type_align64(x)
#define tdef_align64(x)
#define gcc_align64(x)
#elif defined(ARCH_x64)
#define type_align32(x)
#define tdef_align32(x)
#define gcc_align32(x)
#define type_align64(x) type_align(x)
#define tdef_align64(x) tdef_align(x)
#define gcc_align64(x) gcc_align(x)
#endif
