/**
*author:pp
*date:2023/12/16
*/
#ifndef PSTD_THREADPOOL
#define PSTD_THREADPOOL
#ifdef PSTD_EXPORTS
#define PSTD_API __declspec(dllexport)
#else
#define PSTD_API __declspec(dllimport)
#endif
#include<vector>
#include<queue>
#include<memory>
#include<atomic>
#include<thread>
#include<functional>
#include<chrono>
#include<condition_variable>
#include<unordered_map>
#include"result.h"
namespace pstd {
	//任务抽象基类


	//线程池模式
	enum class  PSTD_API PThreadPoolMode {
		MODE_FIXED,//固定数量的线程
		MODE_CACHED//线程数量可动态增长
	};

	//线程类
	class PSTD_API Thread {
	public:
		using ThreadFunc = std::function<void(unsigned long long)>;
		Thread(ThreadFunc func);
		~Thread();
		void start();
		unsigned long long getThreadId();
	private:
		static unsigned long long generateId;
		unsigned long long  threadId;
		ThreadFunc func_;
	};

	//线程池类
	class PSTD_API PThreadPool
	{
	public:
		PThreadPool();
		~PThreadPool();
		PThreadPool(const PThreadPool&) = delete;
		PThreadPool& operator=(const PThreadPool&) = delete;

		void setMode(PThreadPoolMode mode);//设置线程池模式
		void start(int initThreadNum = std::thread::hardware_concurrency());//启动
		void setTaskQueMaxSize(unsigned int size);//设置任务队列最大任务数
		void setMinThreadSize(unsigned int minSize);//设置cached模式下的线程数量下限
		void setMaxThreadSize(unsigned int maxSize);//设置cached模式下的线程数量上限
		Result submitTask(std::shared_ptr<pstd::Task>task);//提交任务

	private:
		void threadFunc(unsigned long long threadId);
		bool checkState();
		//使用unique_ptr能够自动地开辟和释放堆内存
		//std::vector<std::unique_ptr<Thread>>threadVec_;//线程队列
		std::unordered_map<unsigned long long, std::unique_ptr<Thread>>threadMap_;//线程集合
		std::queue<std::shared_ptr<pstd::Task>>taskQue_;//任务队列
		unsigned int taskQueMaxSize_;//任务队列可承受的最大任务数量
		std::atomic_uint taskNum_;//任务数量
		std::atomic_uint threadNum_;//线程数量
		std::atomic_uint threadMaxSize;//cached模式下 线程数量上限
		std::atomic_uint threadMinSize;//cached模式下 线程数量下限
		std::atomic_int freeThreadNum;//空闲线程数量

		std::mutex taskMutex_;//互斥锁
		std::condition_variable cd_notFull_;//不满条件变量
		std::condition_variable cd_notEmpty_;//不空条件变量
		std::condition_variable exitCond_; // 等到线程资源全部回收

		PThreadPoolMode mode_;
		std::atomic_bool isPoolRunning;
	};
}
#endif
