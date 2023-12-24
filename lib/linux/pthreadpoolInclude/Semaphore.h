#ifndef PSTD_SEMAPHORE
#define PSTD_SEMAPHORE

#include<mutex>
#include<condition_variable>
#include<atomic>
namespace pstd {
	class Semaphore
	{
	public:
		Semaphore(int limit = 0) :resLimit_(limit),ifExit(false)
		{

		}
		~Semaphore()
		{
		ifExit=true;
		}
		//�ȴ�һ���ź�����Դ
		void wait()
		{
		        if(ifExit)
		          return;
			std::unique_lock<std::mutex>lock(mtx_);
			//�ȴ���Դʱ�ͷ��� �õ���Դ�����»�ȡ��
			condi_.wait(lock, [&]()->bool {return resLimit_ > 0; });
			resLimit_--;
		}
		//����һ���ź�����Դ
		void post()
		{
		        if(ifExit)
		         return;
			std::unique_lock<std::mutex>lock;
			resLimit_++;
			condi_.notify_all();
		}
	private:
	        std::atomic<bool>ifExit;
		int resLimit_;
		std::mutex mtx_;
		std::condition_variable condi_;
	};
}
#endif

