#pragma once

#include <stdint.h>
#include <cstring>

namespace DEngine {

	struct DataBuffer
	{
		uint8_t* Data = nullptr;
		uint64_t Size = 0;

		DataBuffer() = default;

		DataBuffer(uint64_t size)
		{
			Allocate(size);
		}

		DataBuffer(const void* data, uint64_t size)
			: Data((uint8_t*)data), Size(size)
		{
		}

		DataBuffer(const DataBuffer&) = default;

		static DataBuffer Copy(DataBuffer other)
		{
			DataBuffer result(other.Size);
			memcpy(result.Data, other.Data, other.Size);
			return result;
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = (uint8_t*)malloc(size);
			Size = size;
		}

		void Release()
		{
			free(Data);
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T* As()
		{
			return (T*)Data;
		}

		operator bool() const
		{
			return (bool)Data;
		}

	};

	struct ScopedDataBuffer
	{
		ScopedDataBuffer(DataBuffer buffer)
			: m_Buffer(buffer)
		{
		}

		ScopedDataBuffer(uint64_t size)
			: m_Buffer(size)
		{
		}

		~ScopedDataBuffer()
		{
			m_Buffer.Release();
		}

		uint8_t* Data() { return m_Buffer.Data; }
		uint64_t Size() { return m_Buffer.Size; }

		template<typename T>
		T* As()
		{
			return m_Buffer.As<T>();
		}

		operator bool() const { return m_Buffer; }
	private:
		DataBuffer m_Buffer;
	};
}