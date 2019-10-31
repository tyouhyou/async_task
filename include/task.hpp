#include "thread_pool.hpp"

namespace zb 
{
    template<typename TRESULT>
    class async_task;

    class task
    {
    public:

        template <typename CALLABLE, typename... ARGS>
        static auto async(CALLABLE&& fun, ARGS&&... args)
        ->async_task<decltype(fun(args...))>
        {
            using T = decltype(fun(args...));
            async_task<T> tsk(task::pool_->run(fun, args...));
            return std::move(tsk);
        }

    private:
        static thread_pool::ptr pool_;
    };
}