
# pBoost
a A lightweight third-party CPP library

**[A tiny boost library in C++11.](https://github.com/ppywj/pBoost)**


## pThreadPool
一个基于c++11的轻量级线程池库,支持两种模式fixed模式（固定线程数量），cached模式（动态线程数量），命名控件均为pstd
包括类：
Any(Base Drive 两个内部类）:上帝类，类似于java种的OBject是所有数据类型的基类
Thread:线程类
PThreadPool：线程池类
Task:任务类
Result：结果类，用于保存线程执行结果
Semaphonre：信号量类，基于互斥锁和条件变量实现

use example:
```cpp
#include"pthreadpool.h"
#include<chrono>
#include<thread>
#include"Any.h"
#include<iostream>
using uLLong = unsigned long long;
class myTask :public pstd::Task
{
	int start_;
	int end_;
public:
	myTask(int start,int end):start_(start),end_(end)
	{}
	virtual pstd::Any run()
	{
		uLLong res = 0;
		for (uLLong i = start_; i <= end_; i++)
		{
			res += i;
		}
		std::this_thread::sleep_for(std::chrono::seconds(2));
		printf("结果%llu\n", res);
		return res;
	}
};
int main()
{
		pstd::PThreadPool threadPool;
		threadPool.setMode(pstd::PThreadPoolMode::MODE_CACHED);
		threadPool.setMaxThreadSize(24);
		threadPool.setMinThreadSize(std::thread::hardware_concurrency());
		threadPool.start(12);
    pstd::Result r1 = threadPool.submitTask(std::make_shared<myTask>(1, 100000000));
    pstd::Result r2 =threadPool.submitTask(std::make_shared<myTask>(100000001, 200000000));
    pstd::Result r3 = threadPool.submitTask(std::make_shared<myTask>(200000001, 300000000));
    uLLong res1 = 0;
    res1 += r1.get().cast_<uLLong>();
    res1 += r2.get().cast_<uLLong>();
    res1 += r3.get().cast_<uLLong>();
    uLLong res = 0;
    for (int i = 1; i <= 300000000; i++)
   	res += i;
    std::cout << "res=" << res << std::endl;
    std::cout << "res1=" << res1 << std::endl;
   	getchar();
	  return 0;
}
```
