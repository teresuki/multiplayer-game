#include "Serializer.h"


using Buffer = std::vector<char>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

template <typename NetStructType>
std::unique_ptr<char[]> Serialize(const NetStructType& InStruct)
{
	Buffer payload{};

	uint64_t payloadSize = bitsery::quickSerialization<OutputAdapter>(payload, InStruct);

	// Let's define our packet as follow.
	// -----------------------------------------------------------------------
	// |					|				  |
	// |	PacketEnum		| Payload Size    |		Payload
	// |					|				  |
	// | <---------------->  <-------------->	<---------------------------->
	// | (uint32) 4 bytes     (uint64_t) 8 bytes			Payload Size
	//------------------------------------------------------------------------

	const size_t bufferSize = sizeof(NetPacketType) + sizeof(payloadSize) + payloadSize;
	//char* buffer = new char[bufferSize];
	auto sendingBuffer = std::make_unique<char[]>(bufferSize);

	const auto type = NetPacketType::MyStruct;

	// Copy the bytes of the enum value into the buffer
	std::memcpy(sendingBuffer.get(), &type, sizeof(NetPacketType));

	// Copy the bytes of the payload size value into the buffer;
	std::memcpy(sendingBuffer.get() + sizeof(NetPacketType), &payloadSize, sizeof(payloadSize));

	// Finally, Copy the payload
	std::memcpy(sendingBuffer.get() + sizeof(NetPacketType) + sizeof(payloadSize), payload.data(), payloadSize);

	return sendingBuffer;
}


template std::unique_ptr<char[]> Serialize(const MySmallerStruct& InStruct);
template std::unique_ptr<char[]> Serialize(const MyStruct& InStruct);


template <typename NetStructType>
void Deserialize(NetStructType& OutStruct, const char* ReceivingBuffer)
{
	uint64_t PayloadSize{};
	std::memcpy(&PayloadSize, ReceivingBuffer + sizeof(NetPacketType), sizeof(PayloadSize));

	Buffer payload(PayloadSize);
	std::copy
	(ReceivingBuffer + sizeof(NetPacketType) + sizeof(PayloadSize),
		ReceivingBuffer + sizeof(NetPacketType) + sizeof(PayloadSize) + PayloadSize,
		payload.begin());

	auto state = bitsery::quickDeserialization<InputAdapter>({ payload.begin(), PayloadSize }, OutStruct);
}


template void Deserialize(MySmallerStruct& OutStruct, const char* ReceivingBuffer);
template void Deserialize(MyStruct& OutStruct, const char* ReceivingBuffer);


using StdMap = bitsery::ext::StdMap;
template<typename S>
void serialize(S& s, std::map<int32, std::string>& o)
{
	s.ext(o,
		StdMap{ 100'000 },
		[](S& s, int32& key, std::string& value)
		{
			s.value4b(key);
			s.text1b(value, 100'000);
		}
	);
}


template<typename S>
void serialize(S& s, MySmallerStruct& o)
{
	s.container8b(o.multipleInts, 100'000);
}


template<typename S>
void serialize(S& s, MyStruct& o)
{
	s.value4b(o.myInt);
	s.value2b(o.myEnum);
	s.text1b(o.myString, 100'000);
	s.container4b(o.myFloats, 100'000);
	s.object(o.mySmallerStruct);
	serialize(s, o.myIntStringMap);
};