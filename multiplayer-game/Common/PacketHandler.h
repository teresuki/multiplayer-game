#include "CommonHeader.h"
#include "Serializer.h"
#include "MapSerializer.h"
#include "NetStructExampleSerializer.h"
#include "PacketName.h"


template <typename ReceivedPacket>
requires std::derived_from<ReceivedPacket, Packet>
static void OnPacketIdentifiedAs(const char* receivedBuffer)
{
	ReceivedPacket ReceivingPacket{};
	auto [bitseryResult, isSuccess] = Deserialize(ReceivingPacket, receivedBuffer);
	if (bitseryResult != bitsery::ReaderError::NoError || !isSuccess)
	{
		std::cerr << "DESERIALIZATION FAILED";
		return;
	}

	OnPacketDeserializedAs<ReceivedPacket>(ReceivingPacket);
}

static PacketName IdentifyPacket(const char* receivedBuffer)
{
	PacketName type{};
	std::memcpy(&type, receivedBuffer, sizeof(PacketName));
	return type;
}


template <typename Packet>
static void OnPacketDeserializedAs(const Packet& ReceivedPacket)
{

}

template <>
static void OnPacketDeserializedAs<MyPacket>(const MyPacket& ReceivedPacket)
{
	std::cout << "Received MyStruct packet from Client!\n";
}

static const std::map<PacketName, std::function<void(char*)>> PacketHandlerMap
{
	std::make_pair(PacketName::MyPacket, std::bind(OnPacketIdentifiedAs<MyPacket>, std::placeholders::_1)),
};
