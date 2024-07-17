#include <iostream>

#include "../../src/utils/thread/thread_pool.hpp"
#include "../../src/utils/calc_runtime.hpp"

void task1(void *param)
{
    atomic<LONG> *a = static_cast<atomic<LONG> *>(param);
    ++(*a);
}
struct submit_task_thread : public thread
{
    void submit_task(thread_pool *pool, atomic<LONG> &a)
    {
        task_func task;
        for (int i = 0; i < 3000000; ++i)
        {
            if (pool)
            {
                task.m_func = task1;
                task.m_param = &a;
                pool->submit(task);
            }
        }
    }

    void run() override { submit_task(&pool, a); }
    submit_task_thread(thread_pool &pool, atomic<LONG> &a) : pool(pool), a(a) {}
    thread_pool &pool;
    atomic<LONG> &a;
};

int main()
{
    typedef shared_ptr<thread> thread_ptr;
    typedef std::list<thread_ptr> threads;

    calc_runtime ct;
    thread_pool pool(4, 0);
    atomic<LONG> count = 0;
    threads listThreads;
    for (int i = 0; i < 3; ++i)
    {
        listThreads.push_back(thread_ptr(new submit_task_thread(pool,count)));
    }
    for (threads::iterator it = listThreads.begin(); it != listThreads.end(); ++it)
    {
        (*it)->join();
    }

    pool.wait();
    ct.end();
    std::cout << "run time: " << ct.run_time_in_second() << " s" << std::endl;
    std::cout << "count = " << count << std::endl;

    return 0;
}