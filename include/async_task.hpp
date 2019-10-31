#pragma once

#include "task.hpp"

namespace zb
{
    template<typename TRESULT = void>
    class async_task
    {
        friend class task;
    public:
        async_task() = delete;

        template <typename R>
        async_task<R> await(std::function<R(TRESULT)> fun)
        {
            TRESULT rst = result_.get();
            return std::move(task::async(fun, rst));
        }

        TRESULT result()
        {
            return std::move(result_.get());
        }

    private:
        async_task(std::future<TRESULT> rst)
        {
            this->result_ = move(rst);
        }
        
        std::future<TRESULT> result_;
    };
}