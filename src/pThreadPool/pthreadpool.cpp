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
			threadMap_[i]->start(); // ��Ҫȥִ��һ���̺߳���
			freeThreadNum++;//�����߳���������
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
		std::cout  << "�����������" << "��ȡ���������\n";
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
		//��ȡ��
		std::unique_lock<std::mutex>locker(taskMutex_);
		//�ж����� ������ȴ���������
		bool ret = cd_notFull_.wait_for(locker, std::chrono::seconds(1), [&]()->bool {return taskNum_ < taskQueMaxSize_; });
		//�ȴ���ʱ
		if (ret == false)
			return Result(nullptr, false);
		//�������
		taskQue_.push(task);
		taskNum_++;
		//֪ͨ����
		cd_notEmpty_.notify_all();
		if (mode_ == pstd::PThreadPoolMode::MODE_CACHED
			&& threadNum_ < threadMaxSize
			&& taskNum_>freeThreadNum)
		{
			std::cout << "�´���һ���߳�" << std::endl;
			//�����߳� 
			auto ptr = std::make_unique<Thread>(std::bind(&PThreadPool::threadFunc, this, std::placeholders::_1));
			unsigned long long id = ptr->getThreadId();
			threadMap_.emplace(id, std::move(ptr));
			//�����߳�
			threadMap_[id]->start();
			//���¿����߳�����
			freeThreadNum++;
			//�����߳�����
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
				//�Ȼ�ȡ��
			//	std::cout << std::this_thread::get_id() << "�̳߳���" << "��ȡ���������\n";
				std::unique_lock<std::mutex>locker(taskMutex_);
				printf("%d�̻߳�ȡ�����������ȡ���������\n", std::this_thread::get_id());
				//�ȴ���������
				while (taskNum_ == 0)
				{

					if (!isPoolRunning)
					{
						threadMap_.erase(threadId); // std::this_thread::getid()
						std::cout << "threadid:" << std::this_thread::get_id() << " exit!"
							<< std::endl;
						exitCond_.notify_all();
						return; // �̺߳����������߳̽���
					}
					if (mode_ == pstd::PThreadPoolMode::MODE_CACHED)
					{
						//cachedģʽ��������߳��������ޣ��߳�60��û������ִ����ô��Ҫ����
						// ������������ʱ������
						if (std::cv_status::timeout ==
							cd_notEmpty_.wait_for(locker, std::chrono::seconds(1)))
						{
							auto now = std::chrono::high_resolution_clock().now();
							auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
							if ((dur.count() >=10)&& threadNum_>threadMinSize)
							{
								// ��ʼ���յ�ǰ�߳�
								// ��¼�߳���������ر�����ֵ�޸�
								// ���̶߳�����߳��б�������ɾ��   û�а취 threadFunc��=��thread����
								// threadid => thread���� => ɾ��
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
				//ȡ��һ������
				task = taskQue_.front();
				taskQue_.pop();
				//�����������-1
				taskNum_--;
				//���������֪ͨ����
				if (taskNum_ > 0)
					cd_notEmpty_.notify_all();
				//֪ͨ������в���
				cd_notFull_.notify_all();
			}//���������ͷ���

			//ִ���������
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