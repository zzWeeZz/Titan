#include "TNpch.h"
#include "ThreadPool.h"

namespace Titan
{
	
	void ThreadPool::Initialize(std::function<void(std::string message, Severity severity)>&& messageCallback)
	{
		/*s_Logger = messageCallback;
		Log("Logger linked!", Severity::Log);
		auto threadCount = std::thread::hardware_concurrency();
		for (size_t i = 0; i < threadCount; ++i)
		{
			ThreadInfo info;
			info.thread = new std::thread(ThreadLoop);
			info.status = ThreadStatus::Idle;
			s_Threads[info.thread->get_id()] = info;
		}*/

	}
	void ThreadPool::Shutdown()
	{
	/*	for (auto& thrID : s_Threads)
		{
			thrID.second.thread->join();
		}

		for (auto& thrID : s_Threads)
		{
			delete thrID.second.thread;
			thrID.second.thread = nullptr;
		}

		s_Threads.clear();*/
	}
	void ThreadPool::Log(const std::string& messsage, Severity severity)
	{
		std::lock_guard<std::mutex> lock(s_LogMutex);
		s_Logger(messsage, severity);
	}
	void ThreadPool::ThreadLoop()
	{
		auto& localInfo = s_Threads[std::this_thread::get_id()];
		while (true)
		{
			std::unique_lock lk(s_GlobalThreadLock);
			localInfo.status = ThreadStatus::Idle;
			s_Condition.wait(lk, [] { return !s_Tasks.empty(); });
			std::stringstream ss;
			ss << std::this_thread::get_id();
			Log("Thread: " + ss.str() + " has started working.", Severity::Log);
			auto tsk = s_Tasks.front();
			s_Tasks.pop();
			localInfo.status = ThreadStatus::Working;
			tsk();
		}
	}
}
