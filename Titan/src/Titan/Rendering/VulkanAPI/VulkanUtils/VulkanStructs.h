#pragma once
#include <deque>
#include <functional>



struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;
	void PushFunction(std::function<void()>&& func)
	{
		deletors.push_back(func);
	}

	void Flush()
	{
		for (auto it = deletors.begin(); it != deletors.end(); ++it)
		{
			(*it)();
		}
		deletors.clear();
	}
};
