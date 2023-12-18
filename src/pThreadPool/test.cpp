/**
*author:pp
*date:2023/12/16
*/

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
		printf("½á¹û%llu\n", res);
		return res;
	}
};
int main()
{
	{
		pstd::PThreadPool threadPool;
		threadPool.setMode(pstd::PThreadPoolMode::MODE_CACHED);
		threadPool.setMaxThreadSize(24);
		threadPool.setMinThreadSize(std::thread::hardware_concurrency());
		threadPool.start(12);
		threadPool.submitTask(std::make_shared<myTask>(200000001, 300000000));
		threadPool.submitTask(std::make_shared<myTask>(200000001, 300000000));
		threadPool.submitTask(std::make_shared<myTask>(200000001, 300000000));
		threadPool.submitTask(std::make_shared<myTask>(200000001, 300000000));
		threadPool.submitTask(std::make_shared<myTask>(200000001, 300000000));
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	//pstd::Result r1 = threadPool.submitTask(std::make_shared<myTask>(1, 100000000));
	//pstd::Result r2 =threadPool.submitTask(std::make_shared<myTask>(100000001, 200000000));
	//pstd::Result r3 = threadPool.submitTask(std::make_shared<myTask>(200000001, 300000000));
	//uLLong res1 = 0;
	//res1 += r1.get().cast_<uLLong>();
	//res1 += r2.get().cast_<uLLong>();
	//res1 += r3.get().cast_<uLLong>();
	//uLLong res = 0;
	//for (int i = 1; i <= 300000000; i++)
	//	res += i;
	//std::cout << "res=" << res << std::endl;
	//std::cout << "res1=" << res1 << std::endl;

	getchar();
	return 0;
}