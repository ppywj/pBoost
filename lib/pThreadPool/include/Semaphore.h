/**
*author:pp
*date:2023/12/16
*/
#ifndef PSTD_SEMAPHORE
#define PSTD_SEMAPHORE
#ifdef PSTD_EXPORTS
#define PSTD_API __declspec(dllexport)
#else
#define PSTD_API __declspec(dllimport)
#endif
#include<mutex>
#include<condition_variable>
namespace pstd {
	class PSTD_API Semaphore
	{
	public:
		Semaphore(int limit = 0) :resLimit_(limit)
		{

		}
		~Semaphore() = default;
		//�ȴ�һ���ź�����Դ
		void wait()
		{
			std::unique_lock<std::mutex>lock(mtx_);
			//�ȴ���Դʱ�ͷ��� �õ���Դ�����»�ȡ��
			condi_.wait(lock, [&]()->bool {return resLimit_ > 0; });
			resLimit_--;
		}
		//����һ���ź�����Դ
		void post()
		{
			std::unique_lock<std::mutex>lock;
			resLimit_++;
			condi_.notify_all();
		}
	private:
		int resLimit_;
		std::mutex mtx_;
		std::condition_variable condi_;
	};
}
#endif

