#include "thread_pool.hpp"


namespace zb 
{    
    struct _void_ {};
    
    template<typename TRESULT>
    class async_task;

    class task
    {
    public:

        template <typename CALLABLE, typename... ARGS>
        static auto 
        async(CALLABLE&& fun, ARGS&&... args)
        ->async_task<decltype(fun(args...))>
        {
            using T = decltype(fun(args...));
            async_task<T> tsk(task::pool_->run(fun, args...));
            return std::move(tsk);
        }

        template <typename T>
        static void wait(const async_task<T> &tk)
        {
            tk.future_.wait();
        }

    private:
        static thread_pool::ptr pool_;
    };
}