#pragma once
#include <tracy/Tracy.hpp>

template <typename T>
class Singleton
{
protected:
	Singleton() {};
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static T& instance()
	{
		ZoneScoped;
		static T instance{};
		return instance;
	}
};