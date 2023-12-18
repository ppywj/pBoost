/**
*author:pp
*date:2023/12/16
*/
#ifndef PSTD_RESULT
#define PSTD_RESULT
#ifdef PSTD_EXPORTS
#define PSTD_API __declspec(dllexport)
#else
#define PSTD_API __declspec(dllimport)
#endif
#include"Any.h"
#include"Semaphore.h"
namespace pstd {
	class PSTD_API Result;
	class PSTD_API Task{
	public:
		Task();
		~Task() = default;
		virtual Any run() = 0;
		void exec();
		void setResult(Result* res);
	private:
		Result* res_;
	};

	class PSTD_API Result
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
		std::atomic_bool isValid_;
	};


	
}
#endif

