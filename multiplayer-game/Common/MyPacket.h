#pragma once

#pragma once
#include "CommonHeader.h"
#include "Packet.h"

enum class MyEnum : uint16
{
	eOption1 = 0,
	eOption2 = 1,
	eOption3 = 2
};

struct MySmallerStruct
{
	std::vector<uint64> multipleInts;
};

struct MyPacket : public Packet
{
	uint32 myInt;
	MyEnum myEnum;
	std::u8string myString;
	std::vector<float> myFloats;
	MySmallerStruct mySmallerStruct;

	std::map<int32, std::string> myIntStringMap;

	~MyPacket() = default;
};

