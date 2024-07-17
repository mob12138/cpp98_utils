/**
 * @file condition_variable.hpp
 * @brief C++98 windows下的条件变量实现
 * @author zhengw
 * @date 2024-06-07
 */
#ifndef CONDITION_VARIABLE_HPP
#define CONDITION_VARIABLE_HPP

#include "atomic.hpp"
#include "mutex.hpp"
#include "../win/win_event.hpp"

/**
 * @brief 条件变量
 * @note XP系统下无法使用windows api实现的条件变量，故通过事件实现
 * @note 测试和::SleepConditionVariableCS()的结果基本一致，效率相同
 */
class condition_variable
{
public:
    condition_variable() : m_nWaiters(0), m_eventSignal(NULL, FALSE), m_eventBroadcast(NULL, TRUE)
    {
        m_events.add(m_eventSignal);    // 自动重置事件
        m_events.add(m_eventBroadcast); // 手动重置事件
    }
    virtual ~condition_variable() {}

public:
    /**
     * @brief 等待条件变量，直到被唤醒或超时
     * @param [in] v_lock 互斥锁
     * @param [in] v_dwMilliseconds 超时时间，单位毫秒
     * @return BOOL TRUE 被唤醒，FALSE 超时或其他原因导致的退出
     */
    BOOL wait_for(unique_lock<mutex>& v_lock, DWORD v_dwMilliseconds)
    {
        v_lock.unlock(); // 释放锁

        ++m_nWaiters; // 等待线程数加1

        // 等待事件集合中的事件
        DWORD dwRet = m_events.wait_for(v_dwMilliseconds, FALSE);

        // @brief 等待线程数减1
        // @note 这里新声明一个整型来存储当前等待的线程数，
        //       是为了防止多线程同时走到下面的代码，出现m_nWaiters同时为0导致的多次重置事件的情况
        LONG nWaiters = --m_nWaiters;

        // 若是广播通知，且没有等待线程，则重置广播事件
        if (nWaiters == (LONG)0 && m_eventBroadcast.signaled())
        {
            m_eventBroadcast.reset();
        }

        v_lock.lock(); // 重新获取锁

        // 如果不是事件通知唤醒，则说明超时或其他原因导致的退出
        return win_events::is_wait_for_seted(dwRet);
    }

    /**
     * @brief 等待条件变量，直到被唤醒
     * @param [in] v_lock 互斥锁
     */
    void wait(unique_lock<mutex>& v_lock) { wait_for(v_lock, INFINITE); }

    /**
     * @brief 通知等待线程，唤醒一个
     */
    void notify_one() const
    {
        if (m_nWaiters > (LONG)0)
        {
            m_eventSignal.set();
        }
    }

    /**
     * @brief 通知等待线程，唤醒所有
     */
    void notify_all() const
    {
        if (m_nWaiters > (LONG)0)
        {
            m_eventBroadcast.set();
        }
    }

private:
    atomic<LONG> m_nWaiters;    // 等待线程数，原子变量，无需锁保护
    win_event m_eventSignal;    // 自动重置事件，用于通知单个等待线程
    win_event m_eventBroadcast; // 手动重置事件，用于通知所有等待线程
    win_events m_events;        // 事件集合，用于等待多个事件
};

#endif // CONDITION_VARIABLE_HPP