#include "dpch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace DEngine {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	static uint64_t s_Counter = 0;

	UUID::UUID()
		: m_UUID(++s_Counter)
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}

}