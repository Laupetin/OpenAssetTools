#pragma once

// Undefine any previously defined utilities
#ifdef type_align
#undef type_align
#endif
#ifdef tdef_align
#undef tdef_align
#endif
#ifdef memb_align
#undef memb_align
#endif

#ifdef __zonecodegenerator
#define type_align(x) alignas(x)
#define tdef_align(x) alignas(x)
#define memb_align(x) alignas(x)
#define gcc_align(x)
#else
#ifdef _MSVC_LANG
#define type_align(x) __declspec(align(x))
#define tdef_align(x) __declspec(align(x))
#define memb_align(x) __declspec(align(x))
#define gcc_align(x)
#else
#define type_align(x) __attribute__((__aligned__(x)))
#define tdef_align(x) /*__attribute__((__aligned__(x)))*/
#define memb_align(x) __attribute__((__aligned__(x)))
#define gcc_align(x) __attribute__((__aligned__(x)))
#endif
#endif