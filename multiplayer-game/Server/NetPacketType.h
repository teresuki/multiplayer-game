#pragma once
#include "CommonHeader.h"


enum class NetPacketType : uint32
{
	None = 0,
	MyStruct = 1,
	MyStructArray = 2,
	Max,
};
