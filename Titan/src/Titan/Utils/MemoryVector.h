#pragma once
namespace Titan
{
	template<typename T>
	class MemoryVector
	{
	public:
		MemoryVector();
		MemoryVector(const size_t& preSize);
		
		T& Emplace();
		void Reset();
		T& operator[](const size_t& index);
		
		const size_t Size() { return m_VirtualSize; }

	private:
		size_t m_VirtualSize;
		std::vector<T> m_Container;
	};
	template<typename T>
	inline MemoryVector<T>::MemoryVector()
	{
		m_VirtualSize = 0;
		m_Container.resize(8);
	}
	template<typename T>
	inline MemoryVector<T>::MemoryVector(const size_t& preSize)
	{
		m_VirtualSize = 0;
		m_Container.resize(8);
	}
	template<typename T>
	inline T& MemoryVector<T>::Emplace()
	{
		m_VirtualSize++;
		if (m_VirtualSize >= m_Container.size())
		{
			return m_Container.emplace_back();
		}
		return m_Container[m_VirtualSize - 1];
	}
	template<typename T>
	inline void MemoryVector<T>::Reset()
	{
		m_VirtualSize = 0;
	}
	template<typename T>
	inline T& MemoryVector<T>::operator[](const size_t& index)
	{
		return m_Container[index];
	}

}
