#pragma once

// Modified for OAT
/*
 * Copyright 2016 Józef Kucia for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

namespace oat
{
    const unsigned int DXGI_FORMAT_DEFINED = 1;

    // This is a redefinition to make this enum available on platforms without DirectX headers
    enum DXGI_FORMAT
    {
        DXGI_FORMAT_UNKNOWN = 0x00,
        DXGI_FORMAT_R32G32B32A32_TYPELESS = 0x01,
        DXGI_FORMAT_R32G32B32A32_FLOAT = 0x02,
        DXGI_FORMAT_R32G32B32A32_UINT = 0x03,
        DXGI_FORMAT_R32G32B32A32_SINT = 0x04,
        DXGI_FORMAT_R32G32B32_TYPELESS = 0x05,
        DXGI_FORMAT_R32G32B32_FLOAT = 0x06,
        DXGI_FORMAT_R32G32B32_UINT = 0x07,
        DXGI_FORMAT_R32G32B32_SINT = 0x08,
        DXGI_FORMAT_R16G16B16A16_TYPELESS = 0x09,
        DXGI_FORMAT_R16G16B16A16_FLOAT = 0x0a,
        DXGI_FORMAT_R16G16B16A16_UNORM = 0x0b,
        DXGI_FORMAT_R16G16B16A16_UINT = 0x0c,
        DXGI_FORMAT_R16G16B16A16_SNORM = 0x0d,
        DXGI_FORMAT_R16G16B16A16_SINT = 0x0e,
        DXGI_FORMAT_R32G32_TYPELESS = 0x0f,
        DXGI_FORMAT_R32G32_FLOAT = 0x10,
        DXGI_FORMAT_R32G32_UINT = 0x11,
        DXGI_FORMAT_R32G32_SINT = 0x12,
        DXGI_FORMAT_R32G8X24_TYPELESS = 0x13,
        DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 0x14,
        DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 0x15,
        DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 0x16,
        DXGI_FORMAT_R10G10B10A2_TYPELESS = 0x17,
        DXGI_FORMAT_R10G10B10A2_UNORM = 0x18,
        DXGI_FORMAT_R10G10B10A2_UINT = 0x19,
        DXGI_FORMAT_R11G11B10_FLOAT = 0x1a,
        DXGI_FORMAT_R8G8B8A8_TYPELESS = 0x1b,
        DXGI_FORMAT_R8G8B8A8_UNORM = 0x1c,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 0x1d,
        DXGI_FORMAT_R8G8B8A8_UINT = 0x1e,
        DXGI_FORMAT_R8G8B8A8_SNORM = 0x1f,
        DXGI_FORMAT_R8G8B8A8_SINT = 0x20,
        DXGI_FORMAT_R16G16_TYPELESS = 0x21,
        DXGI_FORMAT_R16G16_FLOAT = 0x22,
        DXGI_FORMAT_R16G16_UNORM = 0x23,
        DXGI_FORMAT_R16G16_UINT = 0x24,
        DXGI_FORMAT_R16G16_SNORM = 0x25,
        DXGI_FORMAT_R16G16_SINT = 0x26,
        DXGI_FORMAT_R32_TYPELESS = 0x27,
        DXGI_FORMAT_D32_FLOAT = 0x28,
        DXGI_FORMAT_R32_FLOAT = 0x29,
        DXGI_FORMAT_R32_UINT = 0x2a,
        DXGI_FORMAT_R32_SINT = 0x2b,
        DXGI_FORMAT_R24G8_TYPELESS = 0x2c,
        DXGI_FORMAT_D24_UNORM_S8_UINT = 0x2d,
        DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 0x2e,
        DXGI_FORMAT_X24_TYPELESS_G8_UINT = 0x2f,
        DXGI_FORMAT_R8G8_TYPELESS = 0x30,
        DXGI_FORMAT_R8G8_UNORM = 0x31,
        DXGI_FORMAT_R8G8_UINT = 0x32,
        DXGI_FORMAT_R8G8_SNORM = 0x33,
        DXGI_FORMAT_R8G8_SINT = 0x34,
        DXGI_FORMAT_R16_TYPELESS = 0x35,
        DXGI_FORMAT_R16_FLOAT = 0x36,
        DXGI_FORMAT_D16_UNORM = 0x37,
        DXGI_FORMAT_R16_UNORM = 0x38,
        DXGI_FORMAT_R16_UINT = 0x39,
        DXGI_FORMAT_R16_SNORM = 0x3a,
        DXGI_FORMAT_R16_SINT = 0x3b,
        DXGI_FORMAT_R8_TYPELESS = 0x3c,
        DXGI_FORMAT_R8_UNORM = 0x3d,
        DXGI_FORMAT_R8_UINT = 0x3e,
        DXGI_FORMAT_R8_SNORM = 0x3f,
        DXGI_FORMAT_R8_SINT = 0x40,
        DXGI_FORMAT_A8_UNORM = 0x41,
        DXGI_FORMAT_R1_UNORM = 0x42,
        DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 0x43,
        DXGI_FORMAT_R8G8_B8G8_UNORM = 0x44,
        DXGI_FORMAT_G8R8_G8B8_UNORM = 0x45,
        DXGI_FORMAT_BC1_TYPELESS = 0x46,
        DXGI_FORMAT_BC1_UNORM = 0x47,
        DXGI_FORMAT_BC1_UNORM_SRGB = 0x48,
        DXGI_FORMAT_BC2_TYPELESS = 0x49,
        DXGI_FORMAT_BC2_UNORM = 0x4a,
        DXGI_FORMAT_BC2_UNORM_SRGB = 0x4b,
        DXGI_FORMAT_BC3_TYPELESS = 0x4c,
        DXGI_FORMAT_BC3_UNORM = 0x4d,
        DXGI_FORMAT_BC3_UNORM_SRGB = 0x4e,
        DXGI_FORMAT_BC4_TYPELESS = 0x4f,
        DXGI_FORMAT_BC4_UNORM = 0x50,
        DXGI_FORMAT_BC4_SNORM = 0x51,
        DXGI_FORMAT_BC5_TYPELESS = 0x52,
        DXGI_FORMAT_BC5_UNORM = 0x53,
        DXGI_FORMAT_BC5_SNORM = 0x54,
        DXGI_FORMAT_B5G6R5_UNORM = 0x55,
        DXGI_FORMAT_B5G5R5A1_UNORM = 0x56,
        DXGI_FORMAT_B8G8R8A8_UNORM = 0x57,
        DXGI_FORMAT_B8G8R8X8_UNORM = 0x58,
        DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 0x59,
        DXGI_FORMAT_B8G8R8A8_TYPELESS = 0x5a,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 0x5b,
        DXGI_FORMAT_B8G8R8X8_TYPELESS = 0x5c,
        DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 0x5d,
        DXGI_FORMAT_BC6H_TYPELESS = 0x5e,
        DXGI_FORMAT_BC6H_UF16 = 0x5f,
        DXGI_FORMAT_BC6H_SF16 = 0x60,
        DXGI_FORMAT_BC7_TYPELESS = 0x61,
        DXGI_FORMAT_BC7_UNORM = 0x62,
        DXGI_FORMAT_BC7_UNORM_SRGB = 0x63,
        DXGI_FORMAT_AYUV = 0x64,
        DXGI_FORMAT_Y410 = 0x65,
        DXGI_FORMAT_Y416 = 0x66,
        DXGI_FORMAT_NV12 = 0x67,
        DXGI_FORMAT_P010 = 0x68,
        DXGI_FORMAT_P016 = 0x69,
        DXGI_FORMAT_420_OPAQUE = 0x6a,
        DXGI_FORMAT_YUY2 = 0x6b,
        DXGI_FORMAT_Y210 = 0x6c,
        DXGI_FORMAT_Y216 = 0x6d,
        DXGI_FORMAT_NV11 = 0x6e,
        DXGI_FORMAT_AI44 = 0x6f,
        DXGI_FORMAT_IA44 = 0x70,
        DXGI_FORMAT_P8 = 0x71,
        DXGI_FORMAT_A8P8 = 0x72,
        DXGI_FORMAT_B4G4R4A4_UNORM = 0x73,

        DXGI_FORMAT_P208 = 0x82,
        DXGI_FORMAT_V208 = 0x83,
        DXGI_FORMAT_V408 = 0x84,

        DXGI_FORMAT_FORCE_UINT = 0xffffffff,
    };
} // namespace oat
