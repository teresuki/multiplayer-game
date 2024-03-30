#pragma once

#pragma once
#include "CommonHeader.h"
#include "PacketName.h"

// Serialization - Bitsery
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>
#include <bitsery/ext/std_map.h>

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>


// NetStruct
#include "Packet.h"
#include "MyPacket.h"


///
/// public Serialize and Deserialize function that user uses.
/// 

template <typename SendingPacket>
requires std::derived_from<SendingPacket, Packet>
std::unique_ptr<char[]> Serialize(const SendingPacket& InStruct)
{
	using Buffer = std::vector<char>;
	using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;

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

	const size_t bufferSize = sizeof(PacketName) + sizeof(payloadSize) + payloadSize;
	//char* buffer = new char[bufferSize];
	auto sendingBuffer = std::make_unique<char[]>(bufferSize);

	const auto type = PacketName::MyPacket;

	// Copy the bytes of the enum value into the buffer
	std::memcpy(sendingBuffer.get(), &type, sizeof(PacketName));

	// Copy the bytes of the payload size value into the buffer;
	std::memcpy(sendingBuffer.get() + sizeof(PacketName), &payloadSize, sizeof(payloadSize));

	// Finally, Copy the payload
	std::memcpy(sendingBuffer.get() + sizeof(PacketName) + sizeof(payloadSize), payload.data(), payloadSize);

	return sendingBuffer;
}

template <typename ReceivingPacket>
requires std::derived_from<ReceivingPacket, Packet>
auto Deserialize(ReceivingPacket& OutPacket, const char* ReceivingBuffer)
{
	using Buffer = std::vector<char>;
	using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

	uint64_t PayloadSize{};
	std::memcpy(&PayloadSize, ReceivingBuffer + sizeof(PacketName), sizeof(PayloadSize));

	Buffer payload(PayloadSize);
	std::copy
	(ReceivingBuffer + sizeof(PacketName) + sizeof(PayloadSize),
		ReceivingBuffer + sizeof(PacketName) + sizeof(PayloadSize) + PayloadSize,
		payload.begin());

	return bitsery::quickDeserialization<InputAdapter>({ payload.begin(), PayloadSize }, OutPacket);
}
