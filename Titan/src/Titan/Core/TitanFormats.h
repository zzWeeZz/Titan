#pragma once

namespace Titan
{
	enum class DepthState
	{
		Off = 0,
		ReadWrite,
		ReadOnly,
		Count
	};

	enum class CullState
	{
		None,
		Front,
		Back,
	};

	enum class Topology
	{
		PointList = 0,
		LineList = 1,
		LineStrip = 2,
		TriangleList = 3,
		TriangleStrip = 4,
		TriangleFan = 5,
		LineListAdj = 6,
		LineStripAdj = 7,
		TriangleListAdj = 8,
		TriangleStripAdj = 9,
		PatchList = 10,
	};

#pragma region Samplers
	
	enum class Filter
	{
		Nearest = 0,
		Linear = 1,
	};
	enum class Address
	{
		Repeat = 0,
		MirroredRepeat = 1,
		ClampToEdge = 2,
		ClampToBorder = 3,
		MirrorClampToEdge = 4,
	};
	enum class MipmapMode
	{
		Nearest = 0,
		Linear = 1,
	};

#pragma endregion

#pragma region ImageFormats
	enum class ImageFormat
	{
		Undefined = 0,
		R4G4UN_PACK8 = 1,
		R4G4B4A4_UN_PACK16 = 2,
		B4G4R4A4_UN_PACK16 = 3,
		R5G6B5_UN_PACK16 = 4,
		B5G6R5_UN_PACK16 = 5,
		R5G5B5A1_UN_PACK16 = 6,
		B5G5R5A1_UN_PACK16 = 7,
		A1R5G5B5_UN_PACK16 = 8,
		R8_UN = 9,
		R8_SN = 10,
		R8_US = 11,
		R8_SS = 12,
		R8_UI = 13,
		R8_S = 14,
		R8_SRGB = 15,
		R8G8_UN = 16,
		R8G8_SN = 17,
		R8G8_US = 18,
		R8G8_SS = 19,
		R8G8_UI = 20,
		R8G8_SI = 21,
		R8G8_SRGB = 22,
		R8G8B8_UN = 23,
		R8G8B8_SN = 24,
		R8G8B8_US = 25,
		R8G8B8_SS = 26,
		R8G8B8_UI = 27,
		R8G8B8_SI = 28,
		R8G8B8_SRGB = 29,
		B8G8R8_UN = 30,
		B8G8R8_SN = 31,
		B8G8R8_US = 32,
		B8G8R8_SS = 33,
		B8G8R8_UI = 34,
		B8G8R8_SI = 35,
		B8G8R8_SRGB = 36,
		R8G8B8A8_UN = 37,
		R8G8B8A8_SN = 38,
		R8G8B8A8_US = 39,
		R8G8B8A8_SS = 40,
		R8G8B8A8_UI = 41,
		R8G8B8A8_SI = 42,
		R8G8B8A8_SRGB = 43,
		B8G8R8A8_UN = 44,
		B8G8R8A8_SN = 45,
		B8G8R8A8_US = 46,
		B8G8R8A8_SS = 47,
		B8G8R8A8_UI = 48,
		B8G8R8A8_SI = 49,
		B8G8R8A8_SRGB = 50,
		A8B8G8R8_UN_PACK32 = 51,
		A8B8G8R8_SN_PACK32 = 52,
		A8B8G8R8_US_PACK32 = 53,
		A8B8G8R8_SS_PACK32 = 54,
		A8B8G8R8_UI_PACK32 = 55,
		A8B8G8R8_SI_PACK32 = 56,
		A8B8G8R8_SRGB_PACK32 = 57,
		A2R10G10B10_UN_PACK32 = 58,
		A2R10G10B10_SN_PACK32 = 59,
		A2R10G10B10_US_PACK32 = 60,
		A2R10G10B10_SS_PACK32 = 61,
		A2R10G10B10_UI_PACK32 = 62,
		A2R10G10B10_SI_PACK32 = 63,
		A2B10G10R10_UN_PACK32 = 64,
		A2B10G10R10_SN_PACK32 = 65,
		A2B10G10R10_US_PACK32 = 66,
		A2B10G10R10_SS_PACK32 = 67,
		A2B10G10R10_UI_PACK32 = 68,
		A2B10G10R10_SI_PACK32 = 69,
		R16_UN = 70,
		R16_SN = 71,
		R16_US = 72,
		R16_SS = 73,
		R16_UI = 74,
		R16_SI = 75,
		R16_SF = 76,
		R16G16_UN = 77,
		R16G16_SN = 78,
		R16G16_US = 79,
		R16G16_SS = 80,
		R16G16_UI = 81,
		R16G16_SI = 82,
		R16G16_SF = 83,
		R16G16B16_UN = 84,
		R16G16B16_SN = 85,
		R16G16B16_US = 86,
		R16G16B16_SS = 87,
		R16G16B16_UI = 88,
		R16G16B16_SI = 89,
		R16G16B16_SF = 90,
		R16G16B16A16_UN = 91,
		R16G16B16A16_SN = 92,
		R16G16B16A16_US = 93,
		R16G16B16A16_SS = 94,
		R16G16B16A16_UI = 95,
		R16G16B16A16_SI = 96,
		R16G16B16A16_SF = 97,
		R32_UI = 98,
		R32_SI = 99,
		R32_SF = 100,
		R32G32_UI = 101,
		R32G32_SI = 102,
		R32G32_SF = 103,
		R32G32B32_UI = 104,
		R32G32B32_SI = 105,
		R32G32B32_SF = 106,
		R32G32B32A32_UI = 107,
		R32G32B32A32_SI = 108,
		R32G32B32A32_SF = 109,
		R64_UI = 110,
		R64_SI = 111,
		R64_SF = 112,
		R64G64_UI = 113,
		R64G64_SI = 114,
		R64G64_SF = 115,
		R64G64B64_UI = 116,
		R64G64B64_SI = 117,
		R64G64B64_SF = 118,
		R64G64B64A64_UI = 119,
		R64G64B64A64_SI = 120,
		R64G64B64A64_SF = 121,
		B10G11R11_UF_PACK32 = 122,
		E5B9G9R9_UF_PACK32 = 123,
		D16_UN = 124,
		X8_D24_UN_PACK32 = 125,
		D32_SF = 126,
		S8_UI = 127,
		D16_UN_S8_UI = 128,
		D24_UN_S8_UI = 129,
		D32_SF_S8_UI = 130,
		BC1_RGB_UN_BLOCK = 131,
		BC1_RGB_SRGB_BLOCK = 132,
		BC1_RGBA_UN_BLOCK = 133,
		BC1_RGBA_SRGB_BLOCK = 134,
		BC2_UN_BLOCK = 135,
		BC2_SRGB_BLOCK = 136,
		BC3_UN_BLOCK = 137,
		BC3_SRGB_BLOCK = 138,
		BC4_UN_BLOCK = 139,
		BC4_SN_BLOCK = 140,
		BC5_UN_BLOCK = 141,
		BC5_SN_BLOCK = 142,
		BC6H_UF_BLOCK = 143,
		BC6H_SF_BLOCK = 144,
		BC7_UN_BLOCK = 145,
		BC7_SRGB_BLOCK = 146,
		ETC2_R8G8B8_UN_BLOCK = 147,
		ETC2_R8G8B8_SRGB_BLOCK = 148,
		ETC2_R8G8B8A1_UN_BLOCK = 149,
		ETC2_R8G8B8A1_SRGB_BLOCK = 150,
		ETC2_R8G8B8A8_UN_BLOCK = 151,
		ETC2_R8G8B8A8_SRGB_BLOCK = 152,
		EAC_R11_UN_BLOCK = 153,
		EAC_R11_SN_BLOCK = 154,
		EAC_R11G11_UN_BLOCK = 155,
		EAC_R11G11_SN_BLOCK = 156,
		ASTC_4x4_UN_BLOCK = 157,
		ASTC_4x4_SRGB_BLOCK = 158,
		ASTC_5x4_UN_BLOCK = 159,
		ASTC_5x4_SRGB_BLOCK = 160,
		ASTC_5x5_UN_BLOCK = 161,
		ASTC_5x5_SRGB_BLOCK = 162,
		ASTC_6x5_UN_BLOCK = 163,
		ASTC_6x5_SRGB_BLOCK = 164,
		ASTC_6x6_UN_BLOCK = 165,
		ASTC_6x6_SRGB_BLOCK = 166,
		ASTC_8x5_UN_BLOCK = 167,
		ASTC_8x5_SRGB_BLOCK = 168,
		ASTC_8x6_UN_BLOCK = 169,
		ASTC_8x6_SRGB_BLOCK = 170,
		ASTC_8x8_UN_BLOCK = 171,
		ASTC_8x8_SRGB_BLOCK = 172,
		ASTC_10x5_UN_BLOCK = 173,
		ASTC_10x5_SRGB_BLOCK = 174,
		ASTC_10x6_UN_BLOCK = 175,
		ASTC_10x6_SRGB_BLOCK = 176,
		ASTC_10x8_UN_BLOCK = 177,
		ASTC_10x8_SRGB_BLOCK = 178,
		ASTC_10x10_UN_BLOCK = 179,
		ASTC_10x10_SRGB_BLOCK = 180,
		ASTC_12x10_UN_BLOCK = 181,
		ASTC_12x10_SRGB_BLOCK = 182,
		ASTC_12x12_UN_BLOCK = 183,
		ASTC_12x12_SRGB_BLOCK = 184,
	};

	inline bool IsDepth(ImageFormat& format)
	{
		if (format == ImageFormat::D16_UN
			|| format == ImageFormat::D16_UN_S8_UI
			|| format == ImageFormat::D24_UN_S8_UI
			|| format == ImageFormat::D32_SF
			|| format == ImageFormat::D32_SF_S8_UI
			|| format == ImageFormat::X8_D24_UN_PACK32)
		{
			return true;
		}
		return false;
	}
#pragma endregion


	inline VkFormat FormatToVkFormat(const ImageFormat& format)
	{
		return static_cast<VkFormat>(format);
	}
	inline VkSamplerMipmapMode FormatToVkFormat(const MipmapMode& address)
	{
		return static_cast<VkSamplerMipmapMode>(address);
	}
	inline VkFilter FormatToVkFormat(const Filter& address)
	{
		return static_cast<VkFilter>(address);
	}
	inline VkSamplerAddressMode FormatToVkFormat(const Address& address)
	{
		return static_cast<VkSamplerAddressMode>(address);
	}
	inline VkPrimitiveTopology FormatToVkFormat(const Topology& topology)
	{
		return static_cast<VkPrimitiveTopology>(topology);
	}


	inline ImageFormat VkFormatToFormat(const VkFormat& format)
	{
		return static_cast<ImageFormat>(format);
	}
	inline Filter VkFormatToFormat(const VkFilter& address)
	{
		return static_cast<Filter>(address);
	}
	inline MipmapMode VkFormatToFormat(const VkSamplerMipmapMode& address)
	{
		return static_cast<MipmapMode>(address);
	}
	inline Address VkFormatToFormat(const VkSamplerAddressMode& address)
	{
		return static_cast<Address>(address);
	}
	inline Topology VkFormatToFormat(const VkPrimitiveTopology& address)
	{
		return static_cast<Topology>(address);
	}
}