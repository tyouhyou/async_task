#include <iostream>
#include "async_task.hpp"

using namespace zb;
using namespace std;

int hello(int ini, int count);
int goaway(int count);
void test_expand();
void test_inexpansible();
async_task<string> test_await_result_1();
async_task<> test_await_result_2();
async_task<> test_await_3();

int main()
{
    auto tk = task::async([]{
        this_thread::sleep_for(1000ms);
        cout << "I'll go second on thread " << this_thread::get_id() << endl;
    })
    .await([]()->void{
        cout << "I'll go third on thread " << this_thread::get_id() << endl;
    });
    cout << "I'll go first on thread " << this_thread::get_id() << endl;

    auto tar1 = test_await_result_1();
    auto tar2 = test_await_result_2();
    auto tar3 = test_await_3();

    cout << "I'm waiting on thread " << this_thread::get_id() << endl;

    task::wait<void>(tar2);
    task::wait<void>(tar3);
    cout << tar1.result() << endl;

    task::wait(tk);
}

int hello(int ini, int count) {
    int ret = ini;
    std::this_thread::sleep_for(500ms);
    for (int i = 0; i < count; i++) {
        ret ++;
    }
    cout << "hello, ret = " << ret << " on thread " << this_thread::get_id() << endl;
    return ret;
};

int goaway(int count) {
    int ret = 1000;
    for (int i =0; i < count; i++) {
        ret ++;
    }
    cout << "go away, ret = " << ret << " on thread " << this_thread::get_id() << endl;
    return ret;
};

void test_expand()
{
    cout << "test_expand" << endl;

    auto pool = zb::thread_pool::pool_ptr(1);
    auto f1 = pool->run(hello, 100, 8);
    auto f2 = pool->run(goaway, 5);

    int ret2 = f2.get();
    cout << "count -> " << ret2 << endl;
    int ret1 = f1.get();
    cout << "count -> " << ret1 << endl;

    pool->shutdown();
}

void test_inexpansible()
{
    cout << "test_inexpansible" << endl;

    auto pool = zb::thread_pool::pool_ptr(1, 0);
    auto f1 = pool->run(hello, 118, 9);
    auto f2 = pool->run(goaway, 5);

    int ret2 = f2.get();
    cout << "count -> " << ret2 << endl;
    int ret1 = f1.get();
    cout << "count -> " << ret1 << endl;

    pool->shutdown();
}

async_task<string> test_await_result_1()
{
    return task::async(hello, 3, 4)
           .await_result<int>(goaway)
           .await_result<string>([](int r){
               return "The previous result " + to_string(r);
           });
}

async_task<> test_await_result_2()
{
    return task::async(
        [](int a, int b)->int {
            return a * b;
        }, 3, 4)
        .await_result<string>([](int r){
            cout << "calculate a x b " << " on thread " << this_thread::get_id() << endl;
            return "a x b = " + to_string(r);
        })
        .await_result<void>([](string pr){
            cout << pr << endl;
            cout << "print previous await result a x b " << " on thread " << this_thread::get_id() << endl;
        });
}

async_task<> test_await_3()
{
    async_task<void> a = task::async([]{ cout << "say hello on thread " << this_thread::get_id() << endl; });
    async_task<int> w1 = a.await(goaway, 2);
    async_task<void> w2 = w1.await([](int a, int b){ 
            cout << "a + b = " << a + b << " on thread: " << this_thread::get_id() << endl;
        }, 2, 4);
    return w2;
}
