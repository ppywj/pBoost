/**
*author:pp
*date:2023/12/16
*/
#define PSTD_EXPORTS
#include "Result.h"

pstd::Result::Result(std::shared_ptr<pstd::Task> task, bool isValid):
    task_(task),isValid_(isValid)
{
    if(task_!=nullptr)
    task_->setResult(this);
}

pstd::Any pstd::Result::get()
{
    if (!isValid_)
        return "";
    sem_.wait();
    return std::move(any_);
}

void pstd::Result::setValue(Any any)
{
	// 存储task的返回值
	this->any_ = std::move(any);
	sem_.post(); // 已经获取的任务的返回值，增加信号量资源
}

pstd::Task::Task():res_(nullptr)
{
}

void pstd::Task::exec()
{
    if (res_ != nullptr) {
        res_->setValue(run());
    }
}

void pstd::Task::setResult(Result*res)
{
    res_ = res;
}
