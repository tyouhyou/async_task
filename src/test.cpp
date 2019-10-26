#include <iostream>
#include "thread_pool.hpp"

using namespace zb;
using namespace std;

int hello(int count) {
    int ret = 100;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for (int i =0; i < count; i++) {
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

void test_expand();
void test_inexpansible();

int main() {
    test_expand();
    test_inexpansible();
}

void test_expand() {
    cout << "test_expande" << endl;

    auto pool = thread_pool::pool_ptr(1);
    auto f1 = pool->run(hello, 10);
    auto f2 = pool->run(goaway, 5);

    int ret2 = f2.get();
    cout << "count -> " << ret2 << endl;
    int ret1 = f1.get();
    cout << "count -> " << ret1 << endl;

    pool->shutdown();
}

void test_inexpansible() {
    cout << "test_inexpansible" << endl;

    auto pool = thread_pool::pool_ptr(1, 0);
    auto f1 = pool->run(hello, 10);
    auto f2 = pool->run(goaway, 5);

    int ret2 = f2.get();
    cout << "count -> " << ret2 << endl;
    int ret1 = f1.get();
    cout << "count -> " << ret1 << endl;

    pool->shutdown();
}