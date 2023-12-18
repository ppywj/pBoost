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
	// �洢task�ķ���ֵ
	this->any_ = std::move(any);
	sem_.post(); // �Ѿ���ȡ������ķ���ֵ�������ź�����Դ
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
