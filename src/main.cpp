#include <iostream>
#include <list>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "thread/thread.h"
#include "thread/thread_pool.h"
#include "utils/calc_runtime.h"

void test1(atomic<LONG>& a)
{
    ++a;
}

void submit_task(thread_pool* pool, atomic<LONG>& a)
{
    for (int i = 0; i < 3000000; ++i)
    {
        if (pool)
        {
            pool->submit(boost::bind(test1, boost::ref(a)));
        }
    }
}

int main()
{
    typedef boost::shared_ptr<thread> thread_ptr;
    typedef std::list<thread_ptr> threads;

    calc_runtime ct;
    thread_pool pool(1, 0);
    atomic<LONG> count = 0;
    threads listThreads;
    for (int i = 0; i < 3; ++i)
    {
        listThreads.push_back(boost::make_shared<thread>(boost::bind(submit_task, &pool, boost::ref(count))));
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
