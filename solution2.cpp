#include <iostream>
#include <thread>
#include <mutex>
#include <future>
#include <deque>
#include <functional>

using namespace std;

template <typename Func>
class my_packaged_task;

template<typename ReturnType, typename... Args>
class my_packaged_task<ReturnType(Args...)>
{
    private:
        std::promise<ReturnType> my_promise;
        std::function<ReturnType(Args...)> my_func;
    public:
        my_packaged_task(std::function<ReturnType(Args...)> f) : my_func(f) {}
        void operator()(Args&&... args) {
            try {
            my_promise.set_value(my_func(std::forward<Args&&>(args)...));
            } catch (...) {
            my_promise.set_exception(std::current_exception());
            }
        }

       std::future<ReturnType> get_future() {
        return my_promise.get_future();
       }
};

int main(){
    // std::packaged_task<int(int, int)> t(sqaure);
    my_packaged_task<int(int)> pt([](int x){return x*x;});
    std::future<int> f = pt.get_future();
    pt(10);
    std::cout << f.get() << std::endl;
    return 0;
}