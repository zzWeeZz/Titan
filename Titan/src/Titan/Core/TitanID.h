#pragma once
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#include <windows.h>
#include <iostream>



namespace Titan
{
	typedef UUID TitanID;
	inline TitanID CreateID()
	{
		UUID uuid;
		auto re = UuidCreate(&uuid);
		if (re)
		{
			return uuid;
		}
		return uuid;
	}
	inline TitanID GetIDFromString(const char* str)
	{
		UUID uuid;
		auto re = UuidFromStringA(*(RPC_CSTR*)&str, &uuid);
		if (re)
		{
			return uuid;
		}
		return uuid;
	}
	inline std::string GetStringOfID(TitanID id)
	{
		char* str = nullptr;
		auto re = UuidToStringA(&id, (RPC_CSTR*)&str);
		
		std::string returnValue(str);
		RpcStringFreeA((RPC_CSTR*)&str);
		if (re)
		{
			return returnValue;
		}
		return returnValue;
	}
	struct TitanIDComparer
	{
		bool operator()(const TitanID& Left, const TitanID& Right) const
		{
			// comparison logic goes here
			return memcmp(&Left, &Right, sizeof(Right)) < 0;
		}
	};
}

namespace std
{
	template <>
	struct hash<Titan::TitanID>
	{
		auto operator()(const Titan::TitanID& id) const -> size_t
		{
			static std::hash<unsigned long> hasherLong;
			static std::hash<short> hasherShort;
			static std::hash<char> hasherChar;
			size_t hashedID = 0;
			hashedID ^= (size_t)hasherLong(id.Data1);
			hashedID ^= (size_t)hasherShort(id.Data2);
			hashedID ^= (size_t)hasherShort(id.Data3);
			hashedID ^= (size_t)hasherChar(id.Data4[0]);
			hashedID ^= (size_t)hasherChar(id.Data4[1]);
			hashedID ^= (size_t)hasherChar(id.Data4[2]);
			hashedID ^= (size_t)hasherChar(id.Data4[3]);
			hashedID ^= (size_t)hasherChar(id.Data4[4]);
			hashedID ^= (size_t)hasherChar(id.Data4[5]);
			hashedID ^= (size_t)hasherChar(id.Data4[6]);
			hashedID ^= (size_t)hasherChar(id.Data4[7]);
			return hashedID;
		}
	};
}