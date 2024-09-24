#pragma once

// ===========================================
// This file includes multiple additional definitions
// for ZoneCodeGenerator.
// ===========================================

// Windows Types
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;

namespace std
{
    typedef char int8_t;
    typedef short int16_t;
    typedef int int32_t;
    typedef long long int64_t;
    typedef unsigned char uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int uint32_t;
    typedef unsigned long long uint64_t;
} // namespace std

#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long

// DirectX types do not need to be processed.
// DirectX 9 Types
typedef void IDirect3DVertexDeclaration9;
typedef void IDirect3DPixelShader9;
typedef void IDirect3DVertexShader9;
// DirectX 11 Types
typedef void ID3D11BlendState;
typedef void ID3D11DepthStencilState;
typedef void ID3D11RasterizerState;
typedef void ID3D11ShaderResourceView;
typedef void ID3D11PixelShader;
typedef void ID3D11VertexShader;
typedef void ID3D11InputLayout;
typedef void ID3D11Buffer;

// Unimportant cpp keywords
#define __unaligned
#define volatile
#define __cppobj

// EOF
