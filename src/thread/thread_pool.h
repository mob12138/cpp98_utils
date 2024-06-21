#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <list>

#include "message_queue.h"
#include "thread.h"

class thread_pool
{
    typedef boost::function<void()> task_func; // 任务函数类型
    struct task_wrapper                        // 任务函数包装器
    {
        task_func m_func; // 任务函数
        BOOL m_bStop;     // 停止标志

        task_wrapper(BOOL v_bStop = FALSE) : m_bStop(v_bStop) {}
        task_wrapper(const task_func& v_func) : m_func(v_func), m_bStop(FALSE) {}
        void operator()() const
        {
            if (m_func)
            {
                m_func();
            }
        }
    };
    typedef message_queue<task_wrapper> task_queue; // 任务队列类型
    typedef boost::shared_ptr<thread> thread_ptr;   // 线程指针类型
    typedef std::list<thread_ptr> threads;          // 线程集合类型

public:
    thread_pool(size_t v_nThreadNum = 4, size_t v_nQueueSize = 1000) : m_taskQueue(v_nQueueSize)
    {
        create(v_nThreadNum);
    }
    virtual ~thread_pool() { stop(); }

public:
    void create(size_t v_nThreadNum = 4)
    {
        if (m_listThreads.size() > 0)
        {
            return;
        }

        if (v_nThreadNum == 0)
        {
            v_nThreadNum = 4;
        }
        if (v_nThreadNum > 16)
        {
            v_nThreadNum = 16;
        }

        for (size_t i = 0; i < v_nThreadNum; ++i)
        {
            m_listThreads.push_back(thread_ptr(boost::make_shared<thread>(boost::bind(&thread_pool::run, this))));
        }
    }

    void submit(const task_func& v_func) { m_taskQueue.push_back(task_wrapper(v_func)); }

    void submit_high(const task_func& v_func) { m_taskQueue.push_front(task_wrapper(v_func)); }

    void stop()
    {
        for (size_t i = 0; i < m_listThreads.size(); ++i)
        {
            m_taskQueue.push_front(task_wrapper(TRUE));
        }

        join();
    }

    void wait()
    {
        for (size_t i = 0; i < m_listThreads.size(); ++i)
        {
            m_taskQueue.push_back(task_wrapper(TRUE));
        }

        join();
    }

private:
    void run()
    {
        task_wrapper task;
        while (!task.m_bStop)
        {
            m_taskQueue.pop(task);
            task();
        }
    }

    void join()
    {
        for (threads::iterator it = m_listThreads.begin(); it != m_listThreads.end(); ++it)
        {
            if (*it)
            {
                (*it)->join();
            }
        }

        m_listThreads.clear();
        m_taskQueue.clear();
    }

private:
    threads m_listThreads;
    task_queue m_taskQueue;
};

#endif // THREAD_POOL_H