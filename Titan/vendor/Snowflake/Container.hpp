#pragma once
#include <vector>

namespace Snowflake
{
	template<typename T>
	class Container
	{
	public:
		Container();
		void Push(const T& value);
		void Push(const std::vector<T>& value);

		T& operator[](size_t index);
		T& operator[](uint32_t index);
		T& operator[](int32_t index);

		T Pop();
		size_t Size() const { return m_Size + 1; }
		T* Data();
		void Clear();
	private:
		T m_Data[1024];
		int32_t m_Size;
		constexpr size_t m_MaxSize = 1024;
	};
	template<typename T>
	inline Container<T>::Container()
	{
		memset(&m_Data[0], 0, m_MaxSize * sizeof(T));
		m_Size = -1;
	}
	template<typename T>
	inline void Container<T>::Push(const T& value)
	{
		if (m_Size >= m_MaxSize - 1)
		{
			return;
		}
		m_Size++;
		m_Data[m_Size] = value;
	}
	template<typename T>
	inline void Container<T>::Push(const std::vector<T>& value)
	{
		if (m_Size + value.size() >= m_MaxSize - 1)
		{
			return;
		}
		m_Size++;
		memcpy_s(&m_Data[m_Size], m_MaxSize * sizeof(T), value.data(), value.size() * sizeof(T));
		m_Size += value.size() - 1;
	}
	template<typename T>
	inline T& Container<T>::operator[](size_t index)
	{
		return m_Data[index];
	}
	template<typename T>
	inline T& Container<T>::operator[](uint32_t index)
	{
		return m_Data[static_cast<size_t>(index)];
	}
	template<typename T>
	inline T& Container<T>::operator[](int32_t index)
	{
		return m_Data[static_cast<size_t>(index)];
	}
	template<typename T>
	inline T Container<T>::Pop()
	{
		auto result = m_Data[m_Size];
		m_Size--;
		return result;
	}
	template<typename T>
	inline T* Container<T>::Data()
	{
		return &m_Data[0];
	}
	template<typename T>
	inline void Container<T>::Clear()
	{
		memset(&m_Data[0], 0, m_MaxSize * sizeof(T));
		m_Size = -1;
	}
}