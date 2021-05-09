#pragma once


#include "MiniginPCH.h"
#include "Singleton.h"

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

//source: https://github.com/progschj/ThreadPool

namespace mini
{
	class ThreadPool : public Singleton<ThreadPool>
	{
	public:
		ThreadPool() = default;
		~ThreadPool();

		void Init(size_t threadCount);

		void WaitForAllTasks();

		template<class F, class... Args>
		auto AddTask(F&& f, Args&&... args)
			->std::future<typename std::result_of<F(Args...)>::type>;


	private:
		std::vector< std::thread > workers;
		std::queue< std::function<void()> > tasks;


		std::mutex QueueMutex;
		std::mutex TaskMutex;
		std::condition_variable QueueCondition; //sync for grabbing tasks
		std::condition_variable TaskQueueIsEmpty; //sync for when all tasks are done, just for the WaitForAllTasks function
		bool StopPool = false;


		unsigned int OngoingTasks = 0;
	};

	// init x amount of threads
	inline void ThreadPool::Init(size_t threadCount)
	{
		for (size_t i = 0; i < threadCount; ++i)
			workers.emplace_back([this]
		{
			while(true)
			{
				std::function<void()> task;
				//grab new task
				{
					std::unique_lock<std::mutex> lock(this->QueueMutex);

					this->QueueCondition.wait(lock,	[this]	{ return this->StopPool || !this->tasks.empty(); });

					if (this->StopPool && this->tasks.empty())
						return;

					task = std::move(this->tasks.front());
					this->tasks.pop();
					this->OngoingTasks--;
				}

				task();
				//update task count
				{
					if (this->OngoingTasks == 0)
						TaskQueueIsEmpty.notify_one();
				}
			}
		}
		);
	}

	inline void ThreadPool::WaitForAllTasks()
	{
		std::unique_lock<std::mutex> lock(this->TaskMutex);
		TaskQueueIsEmpty.wait(lock);
	}

	// add new work item to the pool
	template<class F, class... Args>
	auto ThreadPool::AddTask(F&& f, Args&&... args)	-> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> res = task->get_future();

		std::unique_lock<std::mutex> lock(QueueMutex);
		tasks.emplace([task]() { (*task)(); });
		this->OngoingTasks++;

		QueueCondition.notify_one();

		return res;
	}

	// join all Threads
	inline ThreadPool::~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(QueueMutex);
			StopPool = true;
		}
		QueueCondition.notify_all();
		for (std::thread &worker : workers)
			worker.join();
	}
}