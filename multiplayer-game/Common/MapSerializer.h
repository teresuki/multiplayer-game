#pragma once
#include "Serializer.h"

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
