#include <iostream>
#include <thread>
#include <mutex>
#include <future>

using namespace std;
int buffer = 0;
std::mutex m;
std::condition_variable cv;

template <typename T>
class my_future{
    public:
        T get () {
            try {
                std::unique_lock<mutex>locker (m);
                cv.wait(locker, [](){return buffer > 0;});
                T val = buffer;
                cout<<"Cosumed val "<<val<<endl;
                locker.unlock();
                return val;
            } catch (exception_ptr ex)
            {
                rethrow_exception(ex);
            }
        }
};

template <typename T>
class my_promise{
    public:
        shared_ptr<my_future<T>> f;
        exception_ptr ex;
        void set_value(T x){
            std::unique_lock<mutex>locker (m);
            cv.wait(locker, [](){return buffer < 100;});
            buffer = x;
            cout<<"Pushed value "<<x<<" into buffer\n";
            locker.unlock();
            cv.notify_one();
        }

        auto get_future(){
            return *f;
        }

        void set_exception(exception_ptr ex){
            ex = std::current_exception();
        }
};

void task(my_promise<int> buffPromise){
    try
    {
        buffPromise.set_value(1);
    } 
    catch (std::exception&)
    {
        buffPromise.set_exception(std::current_exception());
    }
    
}

int main(){
    // std::thread t1(create_promise);
    my_promise<int> p;
    my_future<int> f = (p.get_future());
    cout << "Promise created \n";
    std::thread tp(task, move(p));
    tp.join();
    cout<<f.get()<<endl;
    return 0;
}