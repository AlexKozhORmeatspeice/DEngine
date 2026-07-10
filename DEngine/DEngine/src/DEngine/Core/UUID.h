#pragma once

namespace DEngine {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& other) : m_UUID(other.m_UUID) {}

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std {
	template <typename T> struct hash;

	template<>
	struct hash<DEngine::UUID>
	{
		std::size_t operator()(const DEngine::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}