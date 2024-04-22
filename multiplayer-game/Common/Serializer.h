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


/// <summary>
/// Text Serialization
/// </summary>
/// 

template <typename T>
concept Text1b = (std::is_convertible_v<T, std::string_view> || std::is_convertible_v<T, std::u8string_view>) &&
	requires (T t) { sizeof(t[0]) == 1; };

template<typename S, Text1b Text>
void serialize(S& s, Text& text)
{
	s.text1b(text, 100'000);
}


/// <summary>
/// Number Serialiation
/// </summary>
/// 

template <typename T>
concept Number = (std::integral<T> || std::floating_point<T> || requires (T t) { std::is_enum_v<T>; }) && !Text1b<T>;

template <typename T>
concept Number1b = Number<T> && sizeof(T) == 1;

template <typename T>
concept Number2b = Number<T> && sizeof(T) == 2;

template <typename T>
concept Number4b = Number<T> && sizeof(T) == 4;

template <typename T>
concept Number8b = Number<T> && sizeof(T) == 8;

template<typename S, Number1b ValueType>
void serialize(S& s, ValueType& value)
{
	s.value1b(value);
}

template<typename S, Number2b ValueType>
void serialize(S& s, ValueType& value)
{
	s.value2b(value);
}

template<typename S, Number4b ValueType>
void serialize(S& s, ValueType& value)
{
	s.value4b(value);
}

template<typename S, Number8b ValueType>
void serialize(S& s, ValueType& value)
{
	s.value8b(value);
}


/// <summary>
/// Vector Serialization
/// </summary>

template<typename T>
concept IsStdVector = std::is_same_v<T, std::vector<typename T::value_type, typename T::allocator_type>>;

template<typename S, IsStdVector VectorType>
void serialize(S& s, VectorType& vec)
{
	s.container(vec, 100'000, []<typename ElementType>(S& s, ElementType& element)
	{
		serialize(s, element);
	});
}

/// <summary>
/// Map Serialization
/// </summary>

using StdMap = bitsery::ext::StdMap;
template<typename S, typename KeyType, typename ValueType>
void serialize(S& s, std::map<KeyType, ValueType>& o)
{
	s.ext(o,
		StdMap{ 100'000 },
		[](S& s, KeyType& key, ValueType& value)
		{
			serialize(s, key);
			serialize(s, value);
		}
	);
}


/// <summary>
/// Generic Packet Serialization
/// </summary>

template <typename PacketType>
concept SerializablePacket
= requires(PacketType packet) { packet.GetTuple(); } && 
(std::derived_from<PacketType, Packet> || std::derived_from<PacketType, PacketComponent>);


template <typename S, SerializablePacket PacketType>
void serialize(S& s, PacketType& packet)
{
	auto tupleRef = packet.GetTuple();
	teresuki::iterate_tuple(tupleRef, [&](auto& element)
	{
		serialize(s, element);
	});
}


/// <summary>
/// public Serialize and Deserialize function that user uses.
/// </summary>

template <typename SendingPacket>
requires std::derived_from<SendingPacket, Packet>

std::tuple< std::unique_ptr<char[]>, size_t> Serialize(const SendingPacket& InStruct)
{
	using Buffer = std::vector<char>;
	using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
	using Context = std::tuple<>;

	Buffer payload{};
	Context ctx{};

	uint64_t payloadSize = bitsery::quickSerialization(ctx, OutputAdapter{ payload }, InStruct);

	// Let's define our packet as follow.
	// -----------------------------------------------------------------------
	// |					|				  |
	// |	PacketEnum		| Payload Size    |				Payload
	// |					|				  |
	// | <---------------->  <-------------->	<---------------------------->
	// | (uint32) 4 bytes     (uint64) 8 bytes			Payload Size
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

	return std::make_tuple(std::move(sendingBuffer), bufferSize);
}

template <typename ReceivingPacket>
requires std::derived_from<ReceivingPacket, Packet>
auto Deserialize(ReceivingPacket& OutPacket, const char* ReceivingBuffer)
{
	using Buffer = std::vector<char>;
	using InputAdapter = bitsery::InputBufferAdapter<Buffer>;
	using Context = std::tuple<>;

	uint64_t PayloadSize{};
	std::memcpy(&PayloadSize, ReceivingBuffer + sizeof(PacketName), sizeof(PayloadSize));

	Buffer payload(PayloadSize);
	std::copy
	(ReceivingBuffer + sizeof(PacketName) + sizeof(PayloadSize),
		ReceivingBuffer + sizeof(PacketName) + sizeof(PayloadSize) + PayloadSize,
		payload.begin());

	Context ctx{};
	return bitsery::quickDeserialization(ctx, InputAdapter{ payload.begin(), PayloadSize }, OutPacket);
}
