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
            using T = decltype(fun(args...));
            auto f = [this, fun, &args...]()->T{
                future_.wait();
                return fun(args...);
            };
            async_task<T> t(thread_pool::pool_->run(f));
            return std::move(t);
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