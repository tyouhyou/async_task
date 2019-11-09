#ASYNC_TASK

A simple c++ asynchonous programming library.

## Introduction

With this library, life would be a bit easier in making async / await style program in c++. Unlike async / await as compiler reserved keywords in other languages, we use methods to achieve the similar goal.
<br /><br />
Say, we can create code like this:
<br />
```
auto t = task::async(hello, "world")
        .await(add, 3, 4)
        .await_result<void>([](int rst){
           cout << "The result of add(3, 4) is " << rst << endl;
        });

...             // other processing on main thread

task.wait(t);   // wait all the asychronuse processing to end.
```
In the above code,<br>
1. Run function [hello] on other thread, It task a string as argument and in this case, the argument is "world".
2. Run function [add] on another thread after hello ends. It returns int value.
3. Run a lambda on thread other than main thread. It waits [add] ends and takes the result of [add] as it's argument. And it returns no value.
4. task.wait blocks the main thread and wait all the above asynchronou processing end.

## classes and API

There has three classes in this library:
- ***thread_pool***
<br>Using thread_pool class is not encouraged. It is the backing class for the tasks to run on. 

- ***async_task***
<br>No instance of async_task can be created directly. You always an instance by calling the static method in task class -> task::async().<br>
With the asyn_task instance, we can use the following methods:
  - await  
    async_task<return_type> await(callable, args...);
    + It runs the callable passed in, on thread from thread_pool. 
    + A callable may be a function pointer, function, lambda, and etc.
    + And the arguments following the callable will be passed to the callable as it's arguments.
    + Also, it can return value.
  <br /><br />
  - await_result  
    async_task<return_type> await_result<return_type>(callable);
    + The previouse asynchronusly run function's reutrn-value will passed the callable as it's argument.
    + The callable should be a function or other callables which take one argument.
    + The callable argument should be the same as the specialized async_task template type. Say, if the the async_task instance is of async_task<int>, so, the callable passed in should has just on argument whose type is int.
    + The callable can also return value. The return type should be specified. Even returns no value, [void] is requires for syntax sake. That is ```await_result<void>(callable) \\callable returns void```
  <br /><br />
  - result  
  TRESULT result();
    + wait the callable (passed to await or await_result) process to end.
    + If the callable has no return value, it return nothing.
    + If the callable has return value, then after get the value, it will be returned.
    + The method should be call 0 or 1 time. Call it multi-times may cause exception.

- ***task***
  <br>
  - async  
  static async_task<return_value> async(callable, args...);
    + This method will create an async_task<return_type> instance.
    + And the callable with it's arguments will be run on a thread from thread_pool.
    + While the async_task instance returned, the callable may not have finished yet.
    You may use async_task.result to wait for callable ends and get the result (if any).
    + Since we get the async_task instance, we can now use (async_task.)await/await_result to chain the processing order.
  <br /><br />
  - wait
  static void wait(async_task<T>);
  + Wait for the async_task ends. Unlike async_task::result, this method does not try to get the result from the asynchronously running function in async_task.

Refer to test.cpp for general usage sample.

***TODO***
add methods
- task::wait_any  
  Wait until any async_task in list ends.
- task::wait_all   
  Wait until all async_task in list end.

***NOTE***
- Since we do not take execution context with the await funtions, when use them, make sure that the callable and argument(s) passed to await should not go out of their scope before await returns. 
- Compile with c++17 or above
- This library has not been fully testes. There have bugs and, use it at your own risk.