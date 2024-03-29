#pragma once
#include "CommonHeader.h"
#include "Serializer.h"

template <typename NetStructType>
static void OnPacketIdentified(char* receivedBuffer)
{
	NetStructType ReceivingStruct{};
	Deserialize(ReceivingStruct, receivedBuffer);

	OnPacketDeserialized<NetStructType>(ReceivingStruct);
}

static NetPacketType IdentifyPacket(char* receivedBuffer)
{
	NetPacketType type{};
	std::memcpy(&type, receivedBuffer, sizeof(NetPacketType));
	return type;
}

template<typename NetStructType>
static void OnPacketDeserialized(NetStructType& ReceivingStruct)
{

}

template<>
static void OnPacketDeserialized<MyStruct>(MyStruct& ReceivingStruct)
{
	std::cout << "Received MyStruct packet from Client!\n";
}

static const std::map<NetPacketType, std::function<void(char*)>> NetPacketHandlerMap
{
	std::make_pair(NetPacketType::MyStruct, std::bind(OnPacketIdentified<MyStruct>, std::placeholders::_1)),
};