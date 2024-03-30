#pragma once
#include "CommonHeader.h"
#include "MyPacket.h"

template<typename S>
void serialize(S& s, MySmallerStruct& o)
{
	s.container8b(o.multipleInts, 100'000);
}


template<typename S>
void serialize(S& s, MyPacket& o)
{
	s.value4b(o.myInt);
	s.value2b(o.myEnum);
	s.text1b(o.myString, 100'000);
	s.container4b(o.myFloats, 100'000);
	s.object(o.mySmallerStruct);
	serialize(s, o.myIntStringMap);
};
