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
    
    /* test async_task with result */
    auto tar1 = test_await_result_1();
    auto tar2 = test_await_result_2();
    auto tar3 = test_await_3();

    cout << tar1.result() << endl;
    task::wait<void>(tar2);
    task::wait<void>(tar3);
}

int hello(int ini, int count) {
    int ret = ini;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for (int i = 0; i < count; i++) {
        cout << "hello, thread " << this_thread::get_id() << " => " << i << endl;
        ret ++;
    }
    return ret;
};

int goaway(int count) {
    int ret = 1000;
    for (int i =0; i < count; i++) {
        cout << "go away, thread " << this_thread::get_id() << " => " << i << endl;
        ret ++;
    }
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
            cout << "calculate a x b " << " at thread: " << this_thread::get_id() << endl;
            return "a x b = " + to_string(r);
        })
        .await_result<void>([](string pr){
            cout << "print a x b " << " at thread: " << this_thread::get_id() << endl;
            cout << pr << endl;
        });
}

async_task<> test_await_3()
{
    return task::async([]{ cout << "say hello" << endl; })
        .await([](int a, int b){ 
            cout << "a + b = " << a + b << " at thread: " << this_thread::get_id() << endl;
        }, 2, 4);
}
