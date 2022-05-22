#include "headers.h"
using namespace std::literals;

class cThreadManager{

    public:
    using signature = std::packaged_task<void()>;
    /* Lets start pool size number of threads which runs worker_pool(infinite loop) */
    cThreadManager(int size){
        for(auto i=0;i<size;++i){
            mthreads.emplace_back([&](){
                worker_pool();
            });
        }
    }
    /* oh let's stop threads from pulling task from packaged_task vector, 
        wait for current threads to finish,
        break from infinate loop
        */
    ~cThreadManager(){
        mstop.store(true);
        std::cout<<"\n the size packaged_tasks::"<<mtasks.mworkers.size()<<std::endl;
        mcv.notify_all();
        for_each(mthreads.begin(),mthreads.end(),mem_fn(&std::thread::join));
    }
    /* Let's add a functor/funtion which has same signature */
    void add_task(signature fun){
        std::lock_guard lk(mtasks.tmtx);
        mtasks.mworkers.push(std::move(fun));
        std::cout<<"\n The size of queue is::"<<mtasks.mworkers.size();
        mcv.notify_one();
    }  

    /* implement async functionality for the given task by returning associated future */
    template <class U>
    auto async(U&& fun){
        
        using Result_type = std::invoke_result_t<std::decay_t<U>>;     
        std::packaged_task<Result_type()> pt(std::forward<U>(fun));
        std::future<Result_type> ft = pt.get_future();
        /* converting int return type funtion to void function in order to 
        push to queue which has signature of type void */
        signature task(
            [pt = std::move(pt)]() mutable{
                pt();
            }
        );
        add_task(std::move(task));
        return (ft);
    }

    /* This function is the critical one which takes task from workers and calls it */
    void worker_pool(){
        std::cout<<"\n This thread id is::"<<std::this_thread::get_id();
        while(true){
            std::unique_lock ul(mtasks.tmtx);
            while(!mstop && mtasks.mworkers.empty() ){
                mcv.wait(ul);
            }
            /* in case, queue is empty */
            //assert(!mtasks.mworkers.empty());
            /* exit if mstop is true */
            if(mstop){
                break;
            }

            auto task = std::move(mtasks.mworkers.front());
            mtasks.mworkers.pop();
            ul.unlock();   // unlock it, why do we want to hold the lock until task closure?
            task();         // execute the front task.

        }

    }
    private:
    struct{
        //std::timed_mutex tmtx; use condition_variable_any if timed_mutex is preferred. 
        std::mutex tmtx;
        std::queue<signature> mworkers;
        } mtasks;
    
    std::vector<std::thread> mthreads;
    std::condition_variable mcv;
    std::atomic<bool> mstop = false;
};

/*
***************************** start main *****************************
std::cout<<"\n start of main";
    {
    cThreadManager tm(4);
    std::vector<std::future<int>> vft;
    std::atomic<int> sum =0;
    for(auto i=0;i<60;++i){
        auto ft =tm.async([i,&sum](){
            sum += i;
            std::this_thread::sleep_for(50ms);
            //std::cout<<"\n the count is::"<<sum<<std::endl;
            return i;
        });
        vft.push_back(std::move(ft));
    }
    
    std::this_thread::sleep_for(4s);
    std::cout<<"\n the value is::"<<vft[42].get();

    }
    std::cout<<"\n end of main";
    ***************************** end main *****************************
*/