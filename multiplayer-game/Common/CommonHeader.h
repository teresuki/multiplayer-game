#pragma once

#include <cstdint>
#include <chrono>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <string>
#include <iostream>
#include <functional>
#include <concepts>
#include <tuple>
#include <fstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>


// Output Parameter
#define OutParam	

#define now() std::chrono::steady_clock::now()

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;


namespace teresuki
{
	template <typename Tuple, typename Callable>
	void iterate_tuple(Tuple&& t, Callable c)
	{
		std::apply(
			[&](auto&... args)
			{
				(c(args), ...);			// Fold expression.
			},
			t);
	}
}


