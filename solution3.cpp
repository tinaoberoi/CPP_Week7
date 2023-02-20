#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

std::condition_variable cv;
bool flag = false;
std::mutex m;

template<typename T>
class my_future{
    public:
        T val;
        my_future(T x):val(x){};
        T get() {
            return val;
        }
};

template<typename T>
class my_promise{
    public:
        T val;
        auto get_future() {
            std::unique_lock<mutex>locker (m);
            cv.wait(locker, [](){return flag;});
            my_future<T> fut(val);
            flag = false;
            locker.unlock();
            cv.notify_one();
            return fut;
        }

        void set_value(T x){
            std::unique_lock<mutex>locker (m);
            cv.wait(locker, [](){return !flag;});
            val = x;
            flag = true;
            locker.unlock();
            cv.notify_one();
        }
};

void task(my_promise<int> buffPromise, const char* threadNum, int loopFor){
    lock_guard<mutex> lock(m);
    buffPromise.set_value(3);
    // m.unlock();
}

int main(){
    my_promise<int> buffPromise;
    my_future<int> buffFuture = buffPromise.get_future();
    std::thread t1(task, move(buffPromise), "T1", 5);
    cout<<"Future value is"<<buffFuture.get()<<endl;
    t1.join();
    return 0;
}