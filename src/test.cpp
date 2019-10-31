#include <iostream>
#include "async_task.hpp"

using namespace zb;
using namespace std;


int hello(int ini, int count);
int goaway(int count);
void test_expand();
void test_inexpansible();
void test_async();

int main() 
{
    // test async_task
    test_async();

    // Test thread_pool 
    test_expand();
    try
    {
        test_inexpansible();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
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

void test_async() 
{
    auto t = task::async(hello, 3, 4)
            .await<int>(goaway)
            .await<string>([](int r){
                return "The previous result " + to_string(r);
            });

    auto r = t.result();
    cout << "Final result : " << r << endl;
}