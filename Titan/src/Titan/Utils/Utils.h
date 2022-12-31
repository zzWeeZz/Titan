#pragma once


namespace Titan
{
	template<typename T>
	void RemoveDuplicates(std::vector<T>& container)
	{
		const size_t size = container.size();
		if (size == 0 || size == 1)
			return;

		// To store index of next unique element
		int32_t j = 0;

		for (int32_t i = 0; i < size - 1; i++)
		{
			if (container[i] != container[i + 1])
			{
				container[j++] = container[i];
			}
		}
			
		container[j++] = container[size - 1];
	};
}