#ifndef PSTD_RESULT
#define PSTD_RESULT

#include"Any.h"
#include"Semaphore.h"
#include<atomic>
namespace pstd {
	class  Result;
	class  Task{
	public:
		Task();
		~Task() = default;
		virtual Any run() = 0;
		void exec();
		void setResult(Result* res);
	private:
		Result* res_;
	};

	class  Result
	{
	public:
		Result(std::shared_ptr<pstd::Task>task, bool isValid = true);
		~Result() = default;
		pstd::Any get();
		void setValue(Any any);
	private:
		pstd::Semaphore semaphore;
		pstd::Any any_;
		pstd::Semaphore sem_;
		std::shared_ptr<pstd::Task>task_;
		std::atomic<bool> isValid_;
	};


	
}
#endif

