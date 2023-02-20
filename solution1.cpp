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
auto my_async(F func, Args... args) { 
    std::packaged_task<int(int, int)> task(func);
    std::future<int> f = task.get_future();
    std::thread t1(std:: move(task), args...);
    t1.detach();
    // std::cout << f.get() << std::endl;
    return f;
}

int main(){
    // auto x = my_async(compute);
    std::future<int> f = my_async(compute, 5, 4);
    cout<<f.get()<<endl;
    // my_async(compute);
}