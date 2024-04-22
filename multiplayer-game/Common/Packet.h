#pragma once
#include "CommonHeader.h"

/// <summary>
/// A pure abstract struct for all packets to inherit from.
/// Does not have any member functions or member data.
/// Its use is only to allow all packets to have the same base type.
/// </summary>
struct Packet
{
	// We want our Packet struct to be a pure abstract class.
	// But we don't have any member functions in this very own struct 
	// that can be used as pure virtual functions. Using the destructor
	// as the pure virtual function is a perfect choice here.
	virtual ~Packet() = 0;
};


/// <summary>
/// A pure abstract struct for all packet components to inherit from.
/// A packet component is a struct that can be serialize but by itself is not a packet that can be sent through network.
/// A packet component is usually a member of a packet.
/// </summary>
struct PacketComponent
{
	virtual ~PacketComponent() = 0;
};


