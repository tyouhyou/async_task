#pragma once

#include "task.hpp"

namespace zb
{
    template<typename TRESULT = void>
    class async_task
    {
    public:
        
        template<typename RET, typename CALLABLE>
        async_task<RET> await_result(CALLABLE&& fun)
        {
            auto f = [fun](std::shared_future<TRESULT> fu)->RET{
                TRESULT rst = fu.get();
                return fun(rst);
            };
            return std::move(task::async(f, future_));
        }

        template<typename CALLABLE, typename... ARGS>
        auto await(CALLABLE&& fun, ARGS&&... args)
        -> async_task<decltype(fun(args...))>
        {
            using T = decltype(fun(args...));
            auto f = [fun, args...](std::shared_future<TRESULT> fu)->T{
                fu.wait();
                return fun(args...);
            };
            return std::move(task::async(f, future_));
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
        friend class task;
        
        async_task() = delete;

        async_task(std::shared_future<TRESULT> rst)
        {
            this->future_ = rst;
        }

        std::shared_future<TRESULT> future_;
    };
}