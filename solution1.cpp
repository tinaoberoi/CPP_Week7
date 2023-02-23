#include <iostream>
#include <thread>
#include <mutex>
#include <future>
#include <deque>

using namespace std;

std::mutex m;
std::condition_variable cv;

int compute(int a, int b) {
  return 42 + a + b;
}

template<typename F, typename ...Args>
auto my_async(F &func, Args... args) { 
    std::packaged_task<F> task(func);
    std::future<decltype(func(args...))> f = task.get_future();
    std::thread t1(std:: move(task), args...);
    t1.detach();
    return f;
}

int main(){
    std::future<int> f = my_async(compute, 5, 4);
    cout<<f.get()<<endl;

    // The whole point of async is that the main thread can continue moving,
    // And the value of the parameter can be received later in the future.
}