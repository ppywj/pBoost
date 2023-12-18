/**
*author:pp
*date:2023/12/16
*/
#define PSTD_EXPORTS
#include "pthreadpool.h"
#include<iostream>
namespace pstd {
#define THREAD_MAX_IDLE_TIME 60;
	unsigned long long Thread::generateId = 0;
	const int defaultTaskQueMaxSize = 1024;

	void PThreadPool::setMode(PThreadPoolMode mode)
	{
		if (checkState())
			return;
		this->mode_ = mode;
	}

	void PThreadPool::start(int initThreadNum)
	{
		if (checkState())
			return;
		isPoolRunning = true;
		this->threadNum_ = initThreadNum;
		for (int i = 0; i < initThreadNum; i++)
		{
			auto ptr = std::make_unique<Thread>(std::bind(&PThreadPool::threadFunc, this,std::placeholders::_1));
			threadMap_.emplace(ptr->getThreadId(), std::move(ptr));
		}
		for (int i = 0; i < threadNum_; i++)
		{
			threadMap_[i]->start(); // 需要去执行一个线程函数
			freeThreadNum++;//空闲线程数量更新
		}

	}

	PThreadPool::PThreadPool() :
		threadMinSize(0),
		threadMaxSize(0),
		taskQueMaxSize_(defaultTaskQueMaxSize),
		taskNum_(0),
		threadNum_(0),
		mode_(PThreadPoolMode::MODE_FIXED),
		isPoolRunning(false),
		freeThreadNum(0)
	{
	}

	PThreadPool::~PThreadPool()
	{
		isPoolRunning = false;
		std::unique_lock<std::mutex>locker(taskMutex_);
		std::cout  << "析构函数获得" << "获取任务队列锁\n";
		cd_notEmpty_.notify_all();
		exitCond_.wait(locker, [&]() -> bool {return threadMap_.size() == 0; });
	}

	void PThreadPool::setTaskQueMaxSize(unsigned int size)
	{
		if (checkState())
			return;
		taskQueMaxSize_ = size;
	}

	void PThreadPool::setMinThreadSize(unsigned int minSize)
	{
		if (isPoolRunning)
			return;
		threadMinSize = minSize;
	}

	void PThreadPool::setMaxThreadSize(unsigned int maxSize)
	{
		if (checkState())
			return;
		threadMaxSize = maxSize;
	}


	pstd::Result PThreadPool::submitTask(std::shared_ptr<Task> task)
	{
		//获取锁
		std::unique_lock<std::mutex>locker(taskMutex_);
		//判断条件 若满则等待不满条件
		bool ret = cd_notFull_.wait_for(locker, std::chrono::seconds(1), [&]()->bool {return taskNum_ < taskQueMaxSize_; });
		//等待超时
		if (ret == false)
			return Result(nullptr, false);
		//添加任务
		taskQue_.push(task);
		taskNum_++;
		//通知不空
		cd_notEmpty_.notify_all();
		if (mode_ == pstd::PThreadPoolMode::MODE_CACHED
			&& threadNum_ < threadMaxSize
			&& taskNum_>freeThreadNum)
		{
			std::cout << "新创建一个线程" << std::endl;
			//创建线程 
			auto ptr = std::make_unique<Thread>(std::bind(&PThreadPool::threadFunc, this, std::placeholders::_1));
			unsigned long long id = ptr->getThreadId();
			threadMap_.emplace(id, std::move(ptr));
			//开启线程
			threadMap_[id]->start();
			//更新空闲线程数量
			freeThreadNum++;
			//更新线程数量
			threadNum_++;
		}
		return Result(task, true);
	}

	void PThreadPool::threadFunc(unsigned long long threadId)
	{
		auto lastTime = std::chrono::high_resolution_clock::now();
		for(;;)
		{
			std::shared_ptr<Task> task(nullptr);
			{
				//先获取锁
			//	std::cout << std::this_thread::get_id() << "线程尝试" << "获取任务队列锁\n";
				std::unique_lock<std::mutex>locker(taskMutex_);
				printf("%d线程获取任务队列锁获取任务队列锁\n", std::this_thread::get_id());
				//等待不空条件
				while (taskNum_ == 0)
				{

					if (!isPoolRunning)
					{
						threadMap_.erase(threadId); // std::this_thread::getid()
						std::cout << "threadid:" << std::this_thread::get_id() << " exit!"
							<< std::endl;
						exitCond_.notify_all();
						return; // 线程函数结束，线程结束
					}
					if (mode_ == pstd::PThreadPoolMode::MODE_CACHED)
					{
						//cached模式如果多于线程数量下限，线程60秒没有任务执行那么就要回收
						// 条件变量，超时返回了
						if (std::cv_status::timeout ==
							cd_notEmpty_.wait_for(locker, std::chrono::seconds(1)))
						{
							auto now = std::chrono::high_resolution_clock().now();
							auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
							if ((dur.count() >=10)&& threadNum_>threadMinSize)
							{
								// 开始回收当前线程
								// 记录线程数量的相关变量的值修改
								// 把线程对象从线程列表容器中删除   没有办法 threadFunc《=》thread对象
								// threadid => thread对象 => 删除
								threadMap_.erase(threadId); // std::this_thread::getid()
								threadNum_--;
								freeThreadNum--;;

								std::cout << "threadid:" << std::this_thread::get_id() << " exit!"
									<< std::endl;
								return;
							}
						}
					}
					else
					{
						cd_notEmpty_.wait(locker);
					}
				}
				freeThreadNum--;
				//取出一个任务
				task = taskQue_.front();
				taskQue_.pop();
				//任务队列数量-1
				taskNum_--;
				//如果不空则通知不空
				if (taskNum_ > 0)
					cd_notEmpty_.notify_all();
				//通知任务队列不满
				cd_notFull_.notify_all();
			}//出作用域释放锁

			//执行这个任务
			if (task != nullptr) {
				task->exec();
				freeThreadNum++;
			}
			lastTime = std::chrono::high_resolution_clock::now();
		}
	}

	bool PThreadPool::checkState()
	{
		return isPoolRunning;
	}

	Thread::Thread(ThreadFunc func) :func_(func), threadId(generateId++)
	{

	}

	Thread::~Thread()
	{
	}

	void Thread::start()
	{
		std::thread t(func_, threadId);
		t.detach();
	}
	unsigned long long Thread::getThreadId()
	{
		return threadId;
	}
}