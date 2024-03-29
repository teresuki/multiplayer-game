#pragma once
#include "CommonHeader.h"
#include "NetPacketType.h"

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
#include "ExampleNetStruct.h"


///
/// public Serialize and Deserialize function that user uses.
/// 

template<typename NetStructType>
std::unique_ptr<char[]> Serialize(const NetStructType& InStruct);

template<typename NetStructType>
void Deserialize(NetStructType& OutStruct, const char* ReceivingBuffer);

