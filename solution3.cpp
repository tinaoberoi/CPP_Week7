#include <iostream>
#include <thread>
#include <mutex>
#include <future>

using namespace std;
std::mutex m;
std::condition_variable cv;

template <typename sT>
struct shared_var
{
   shared_var() : is_done(false), ex(nullptr){};

   bool is_done;
   sT val;
   exception_ptr ex;
};

template <typename T>
class my_promise;

template <typename T>
class my_future{
    private:
        friend class my_promise<T>;
        shared_var<T> *var = new shared_var<T>();
    public:
        T get () {
            if (var->ex) {
                rethrow_exception(var->ex);
            } else {
                std::unique_lock<mutex>locker (m);
                cv.wait(locker, [this](){return var->is_done == true;});
                locker.unlock();
                return var->val;
            }
        }

        my_future() = default;
        my_future(const my_future&) = delete;
        my_future &operator=(const my_future&) = delete;
        my_future(my_future &&) = default;
};

template <typename T>
class my_promise{
    private:
        unique_ptr<my_future<T>> f = make_unique<my_future<T>>();
    public:
        my_promise() = default;
        my_promise(const my_promise &) = delete;
        my_promise &operator=(const my_promise &) = delete;
        my_promise(my_promise &&) = default;

        void set_value(T x){
            try {
                std::unique_lock<mutex>locker (m);
                cv.wait(locker, [this](){return f->var->is_done == false;});
                f->var->is_done = true;
                f->var->val = x;
                locker.unlock();
                cv.notify_one();
            } catch (...)
            {
                set_exception(current_exception());
            }
            
        }

        auto get_future(){
            return move(*f);
        }

        void set_exception(exception_ptr ex){
            f->var->ex = ex;
        }
};

void task(my_promise<int> buffPromise){
    try
    {
        buffPromise.set_value(1234);
    } 
    catch (std::exception&)
    {
        buffPromise.set_exception(std::current_exception());
    }
    
}

int main(){
    my_promise<int> p;
    my_future<int> f = (p.get_future());
    cout << "Promise created \n";
    std::thread tp(task, move(p));
    tp.join();
    cout<<f.get()<<endl;
    return 0;
}