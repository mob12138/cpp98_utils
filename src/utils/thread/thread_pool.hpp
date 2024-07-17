/**
 * @file thread_pool.hpp
 * @brief C++98 线程池
 * @author zhengw
 * @date 2024-07-18
 */

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <list>

#include "message_queue.hpp"
#include "../smart_ptr/shared_ptr.hpp"
#include "thread.hpp"


/**
 * @brief 任务函数
 */
struct task_func
{
    void (*m_func)(void *); // 任务函数
    void *m_param;          // 任务参数

    task_func(void (*v_func)(void *) = NULL, void *v_param = NULL) : m_func(v_func), m_param(v_param) {}
    void operator()() const
    {
        if (m_func)
        {
            m_func(m_param);
        }
    }
};

class thread_pool
{
    struct task_wrapper // 任务包装器
    {
        task_func m_task; // 任务函数
        BOOL m_bStop;     // 停止标志

        task_wrapper(BOOL v_bStop = FALSE) : m_bStop(v_bStop) {}
        task_wrapper(const task_func &v_task) : m_task(v_task), m_bStop(FALSE) {}
        void operator()() const
        {
            if (!m_bStop)
            {
                m_task();
            }
        }
    };
    typedef message_queue<task_wrapper> task_queue; // 任务队列类型
    typedef shared_ptr<thread> thread_ptr;          // 线程指针类型
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
            m_listThreads.push_back(thread_ptr(new exec_task_thread(this)));
        }
    }

    void submit(const task_func &v_func) { m_taskQueue.push_back(task_wrapper(v_func)); }

    void submit_high(const task_func &v_func) { m_taskQueue.push_front(task_wrapper(v_func)); }

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
    struct exec_task_thread : public thread
    {
        exec_task_thread(thread_pool *v_pool) : m_pool(v_pool) {}
        void run()
        {
            if (m_pool) { m_pool->run(); }
        }

    private:
        thread_pool *m_pool;
    };

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

#endif // THREAD_POOL_HPP