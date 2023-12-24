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
	//����������


	//�̳߳�ģʽ
	enum class  PSTD_API PThreadPoolMode {
		MODE_FIXED,//�̶��������߳�
		MODE_CACHED//�߳������ɶ�̬����
	};

	//�߳���
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

	//�̳߳���
	class PSTD_API PThreadPool
	{
	public:
		PThreadPool();
		~PThreadPool();
		PThreadPool(const PThreadPool&) = delete;
		PThreadPool& operator=(const PThreadPool&) = delete;

		void setMode(PThreadPoolMode mode);//�����̳߳�ģʽ
		void start(int initThreadNum = std::thread::hardware_concurrency());//����
		void setTaskQueMaxSize(unsigned int size);//��������������������
		void setMinThreadSize(unsigned int minSize);//����cachedģʽ�µ��߳���������
		void setMaxThreadSize(unsigned int maxSize);//����cachedģʽ�µ��߳���������
		Result submitTask(std::shared_ptr<pstd::Task>task);//�ύ����

	private:
		void threadFunc(unsigned long long threadId);
		bool checkState();
		//ʹ��unique_ptr�ܹ��Զ��ؿ��ٺ��ͷŶ��ڴ�
		//std::vector<std::unique_ptr<Thread>>threadVec_;//�̶߳���
		std::unordered_map<unsigned long long, std::unique_ptr<Thread>>threadMap_;//�̼߳���
		std::queue<std::shared_ptr<pstd::Task>>taskQue_;//�������
		unsigned int taskQueMaxSize_;//������пɳ��ܵ������������
		std::atomic_uint taskNum_;//��������
		std::atomic_uint threadNum_;//�߳�����
		std::atomic_uint threadMaxSize;//cachedģʽ�� �߳���������
		std::atomic_uint threadMinSize;//cachedģʽ�� �߳���������
		std::atomic_int freeThreadNum;//�����߳�����

		std::mutex taskMutex_;//������
		std::condition_variable cd_notFull_;//������������
		std::condition_variable cd_notEmpty_;//������������
		std::condition_variable exitCond_; // �ȵ��߳���Դȫ������

		PThreadPoolMode mode_;
		std::atomic_bool isPoolRunning;
	};
}
#endif
