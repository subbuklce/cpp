#include "headers.h"


template<class F>
class simple_packaged_task{

private:
    std::function<void()> f;
    using Result_type = std::invoke_result_t<std::decay_t<F>>;
    using Promise_type = std::promise<Result_type>;
    using Future_type = std::future<Result_type>;
    Promise_type pr;
    Future_type ft = pr.get_future();
public:
    simple_packaged_task(F& fun){
        f=std::forward<F>(fun);
    }
    void operator()(){
        try{
            Result_type T = f();
            pr.set_value(T);
        }
        catch(...){
            pr.set_exception(std::current_exception());
        }      
        
    }
    auto get_future() const{
        return ft;
    }
    /*
    void task_start(){
        result_type Val = std::async(std::launch::async, f);
        pr.set_value(Val);
    }
    */
};