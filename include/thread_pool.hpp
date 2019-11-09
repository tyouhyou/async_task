#pragma once

#include <thread>
#include <vector>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <algorithm>
#include <queue>
#include <atomic> 

namespace zb 
{
    class thread_pool 
    {
    public:
        using ptr = std::shared_ptr<thread_pool>;
        static 
        ptr pool_ptr(const unsigned int init_size = 16, 
                     const unsigned int increase_size = 8, 
                     const unsigned int max_size = 0) 
        {
            return std::make_shared<thread_pool>(init_size, increase_size, max_size);
        }

        static ptr pool_;

        thread_pool()
            : flag_shut {false}
            , cv {}
            , pool {} 
        {
            init(16, 8, 0);
        }

        /* *
         * @param increase_size
         *          The size to extend the pool size when it drained. 
         *          0 indicates that it's an inextensible pool.
         * @param max_size
         *          The max size to which the pool can extend.
         *          0 indicates that no limit on the max size.
         * */
        explicit 
        thread_pool(const unsigned int init_size, 
                    const unsigned int increase_size = 8, 
                    const unsigned int max_size = 0) noexcept(false) 
                    : flag_shut {false}
                    , cv {}
                    , pool {}
        {
            init(init_size, increase_size, max_size);
        }

        ~thread_pool() 
        {
            if (!pool.empty()) {
                shutdown();
            }
        }

        void shutdown() 
        {
            flag_shut = true;
            cv.notify_all();
            pool.clear();
        }

        template <typename CALLABLE, typename... ARGS>
        auto run(CALLABLE&& fun, ARGS&&... args) noexcept(false)
        -> std::future<decltype(fun(args...))> 
        {
            if (static_cast<unsigned int>(running_count) >= pool_size) {
                expand();
            }

            running_count++;
            using ret_type = decltype(fun(args...));
            auto tp = std::make_shared<std::packaged_task<ret_type()>>(
                std::bind(std::forward<CALLABLE>(fun), std::forward<ARGS>(args)...)
            );
            auto ft = tp->get_future();
            fun_que.push([tp](){(*tp)();});
            cv.notify_one();
            return ft;
        }

    private:
        class func_que 
        {
        public:
            func_que()
            : mtx_{}
            , que_{}
            {}

            void push(std::function<void()> fun) 
            {
                std::lock_guard<std::mutex> lck(mtx_);
                que_.push(fun);
            }
            std::function<void()> pop()
            {
                std::lock_guard<std::mutex> lck(mtx_);
                if (0 == que_.size()) return nullptr;
                auto ret = que_.front();
                que_.pop();
                return std::move(ret);
            }
            bool empty()
            {
                std::lock_guard<std::mutex> lck(mtx_);
                return que_.empty();
            }
        private:
            std::mutex mtx_;
            std::queue<std::function<void()>> que_;
        };

        unsigned int max_size;
        unsigned int increase_size;
        unsigned int pool_size;     // we do not want to lock the pool every time expanding it.
        std::atomic<unsigned int> running_count;

        bool flag_shut;
        std::mutex mtx_pool;
        std::condition_variable cv;

        thread_pool::func_que fun_que;
        std::vector<std::unique_ptr<std::thread>> pool;


        void init(const unsigned int& init_size, 
                  const unsigned int& increase_size, 
                  const unsigned int& max_size)
        {
            if (init_size < 1 || (0 != max_size && init_size > max_size)) {
                throw std::invalid_argument("init_size should be greater than 1 and less than or equal to max_size.");
            }

            this->max_size = max_size;
            this->increase_size = increase_size;
            pool_size = init_size;
            running_count = 0;

            add_thread(init_size);
        }

        void add_thread(const unsigned int& size)
        {
            for (unsigned int i = 0; i < size; ++i) {
                auto thread_ = std::make_unique<std::thread>([this](){
                    std::mutex mtx_;
                    std::unique_lock<std::mutex> lck(mtx_);
                    for(;;) {
                        cv.wait(lck, [this]{ return !fun_que.empty() || flag_shut; });
                        if (flag_shut) break;
                        auto task_ = fun_que.pop();
                        if (!task_) {
                            running_count --;
                            continue;
                        }
                        task_();
                        running_count --;
                    }
                });
                thread_->detach();
                pool.emplace_back(move(thread_));
            }
        }

        void expand()
        {
            std::lock_guard<std::mutex> lock(mtx_pool);
            if (this->increase_size == 0 || (this->max_size != 0 && this->pool_size == this->max_size)) {
                throw std::logic_error("No idle thread available.");
            } else {
                auto sz = 0 == max_size 
                        ? increase_size
                        : max_size - pool_size > increase_size 
                        ? increase_size
                        : max_size - pool_size;
                add_thread(sz);
                pool_size += sz;
            }
        }
    };
}
