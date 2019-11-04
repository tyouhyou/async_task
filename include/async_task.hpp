#pragma once

#include "task.hpp"

namespace zb
{
    template<typename TRESULT = void>
    class async_task
    {
        friend class task;
    public:
        
        template<typename RET, typename CALLABLE>
        async_task<RET> await_result(CALLABLE&& fun)
        {
            TRESULT rst = future_.get();
            return std::move(task::async(fun, rst));
        }

        template<typename CALLABLE, typename... ARGS>
        auto await(CALLABLE&& fun, ARGS&&... args)
        -> async_task<decltype(fun(args...))>
        {
            //TODO: if (future_.valid == false) throw

            future_.wait();
            return std::move(task::async(fun, args...));
        }

        TRESULT result()
        {
            // TODO: protect wait/get from being called multi-times

            if constexpr (std::is_same<TRESULT, void>::value)
            {
                return future_.wait();
            }
            return std::move(future_.get());
        }

    private:
        async_task() = delete;

        async_task(std::future<TRESULT> rst)
        {
            this->future_ = move(rst);
        }

        std::future<TRESULT> future_;
    };
}