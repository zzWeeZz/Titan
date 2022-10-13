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
		UuidCreate(&uuid);
		return uuid;
	}
	inline TitanID GetIDFromString(const char* str)
	{
		UUID uuid;
		UuidFromStringA(*(RPC_CSTR*)&str, &uuid);
		return uuid;
	}
	inline std::string GetStringOfID(TitanID id)
	{
		char* str;
		UuidToStringA(&id, (RPC_CSTR*)&str);
		std::string returnValue(str);
		RpcStringFreeA((RPC_CSTR*)&str);
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
