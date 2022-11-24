#pragma once
#include <vector>
#include <functional>
#include <thread>
#include <future>
#include <string>
#include <map>

#include <condition_variable>
#include <queue>

namespace Titan
{
	enum class ThreadStatus
	{
		Idle,
		Working,
		Overloaded,
		Count,
	};
	struct ThreadInfo
	{
		std::thread* thread;
		ThreadStatus status;

	};
	enum class Severity
	{
		Log,
		Warning,
		Error,
		Count,
	};

	class ThreadPool
	{
	public:
		// if the thead count is -1 hailstorm will take the cpu thead count.
		static void Initialize(std::function<void(std::string message, Severity severity)>&& messageCallback);
		template<class T, class ...Args>
		static auto Dispatch(T&& task, Args&&... args);
		static void Shutdown();
	private:
		static void Log(const std::string& messsage, Severity severity);
		static void ThreadLoop();
		inline static std::mutex s_LogMutex;
		inline static std::mutex s_GlobalThreadLock;
		inline static std::map<std::thread::id, ThreadInfo> s_Threads;
		inline static std::queue<std::function<void()>> s_Tasks;
		inline static std::condition_variable s_Condition;
		inline static std::function<void(std::string message, Severity severity)> s_Logger;
	};
	template<class T, class ...Args>
	inline auto ThreadPool::Dispatch(T&& task, Args && ...args)
	{
		using RetVal = std::invoke_result_t<T&&, Args&&...>;
		auto tsk = std::make_shared<std::packaged_task<RetVal()>>(std::bind(std::forward<T>(task), std::forward<Args>(args)...));

		std::future<RetVal> ftr = tsk->get_future();
		std::unique_lock lock(s_GlobalThreadLock);

		s_Tasks.push([tsk] {(*tsk)(); });
		s_Condition.notify_one();
		return ftr;
	}
}