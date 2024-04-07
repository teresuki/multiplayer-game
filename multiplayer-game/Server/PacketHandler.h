#pragma once
#include <Common/CommonHeader.h>
#include <Common/Serializer.h>
#include <Common/PacketName.h>

static PacketName IdentifyPacket(const char* receivedBuffer)
{
	PacketName type{};
	std::memcpy(&type, receivedBuffer, sizeof(PacketName));
	return type;
}

template <typename ReceivedPacket>
requires std::derived_from<ReceivedPacket, Packet>
static void OnPacketIdentifiedAs(const char* receivedBuffer)
{
	ReceivedPacket ReceivingPacket{};

	auto start = now();
	auto [bitseryResult, isSuccess] = Deserialize(ReceivingPacket, receivedBuffer);
	if (bitseryResult != bitsery::ReaderError::NoError || !isSuccess)
	{
		std::cerr << "DESERIALIZATION FAILED\n";
		return;
	}
	auto end = now();
	std::cout << "Deserialize time: " << (end - start).count() << "ns.\n";

	OnPacketDeserializedAs<ReceivedPacket>(ReceivingPacket);
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
