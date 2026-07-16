#pragma once

namespace DEngine {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& other) : m_UUID(other.m_UUID) 
		{
		}

		UUID(const UUID&& other) : m_UUID(other.m_UUID) 
		{
		}

		operator uint64_t() const { return m_UUID; }
		UUID& operator=(const UUID& other) {
			if (this == &other)
			{
				return *this;
			}

			m_UUID = other.m_UUID;
			return *this;
		}

		static uint64_t Invalid() { return 0; } // 0 считается как не валидный индефикатор для объекта движка
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