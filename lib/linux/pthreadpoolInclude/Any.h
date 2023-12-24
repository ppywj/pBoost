// pstd.h
#ifndef PSTD_ANY
#define PSTD_ANY

#include<memory>
namespace  pstd {
	class  Any
	{
	public:
		template<typename T>
		Any(T val):base_(std::make_unique<Drive<T>>(val))
		{}
		Any() = default;
		~Any() = default;
		Any(const Any& any) = delete;
		Any& operator=(const Any& any) = delete;
		Any(Any&& any) = default;
		Any& operator=( Any&& any) = default;

		template<typename T>
		T cast_()
		{
			Drive<T>* derive = dynamic_cast<Drive<T>*>(base_.get());
			if (derive == nullptr)
			{
				throw "type not match";
			}
			return derive->val_;
		}

		class Base {
		public:
			virtual ~Base() = default;
		};

		template<typename T>
		class Drive :public Base{
		public:
			Drive(T val) :val_(val) {}
			~Drive() = default;
			T val_;
		};
	private:
		std::unique_ptr<Base>base_;
	};
}
#endif
