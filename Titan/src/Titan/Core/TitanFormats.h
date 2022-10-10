#pragma once
#include <d3d12.h>

namespace Titan
{
	enum class ImageFormat
	{
		RGBA32F = 2,
		RGBA32U = 3,
		RGBA32S = 4,
		RGB32F = 6,
		RGB32U = 7,
		RGB32S = 8,
		RGBA16F = 10,
		RGBA16UN = 11,
		RGBA16UI = 12,
		RGBA16SN = 13,
		RGBA16SI = 14,
		RG32F = 16,
		RG32U = 17,
		RG32S = 18,
		RGBA8UN = 28,
		Depth32 = 39,
		R32UI = DXGI_FORMAT_R32_UINT,
		Depth24 = 45,
		R8UN = 61,
	};

	inline DXGI_FORMAT FormatToDXFormat(const ImageFormat& format)
	{
		return static_cast<DXGI_FORMAT>(format);
	}
	inline ImageFormat DXFormatToFormat(const DXGI_FORMAT& format)
	{
		return static_cast<ImageFormat>(format);
	}
	inline bool IsDepth(ImageFormat& format)
	{
		if (format == ImageFormat::Depth24 || format == ImageFormat::Depth32)
		{
			return true;
		}
		return false;
	}



}