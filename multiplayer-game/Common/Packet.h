#pragma once

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

Packet::~Packet() {};
