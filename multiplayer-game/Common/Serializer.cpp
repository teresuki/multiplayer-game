#include "Serializer.h"


using Buffer = std::vector<char>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

//template std::unique_ptr<char[]> Serialize(const MyStruct& InStruct);
//template void Deserialize(MyStruct& OutStruct, const char* ReceivingBuffer);

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