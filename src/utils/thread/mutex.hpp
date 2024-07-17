/**
 * @file mutex.hpp
 * @brief C++98 windows下的互斥量实现
 * @author zhengw
 * @date 2024-05-28
 */

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <windows.h>

/**
 * @brief 局部锁的模板实现
 */
template <class T>
struct unique_lock
{
public:
    unique_lock(T& mutex) : m_mutex(mutex), m_bLocked(FALSE) { lock(); }
    ~unique_lock() { unlock(); }

    void lock()
    {
        if (!m_bLocked)
        {
            m_mutex.lock();
            m_bLocked = TRUE;
        }
    }

    void unlock()
    {
        if (m_bLocked)
        {
            m_mutex.unlock();
            m_bLocked = FALSE;
        }
    }

private:
    T& m_mutex;
    BOOL m_bLocked;
};

/**
 * @brief 互斥量
 */
class mutex
{
public:
    mutex() { ::InitializeCriticalSection(&m_crit); }
    ~mutex() { ::DeleteCriticalSection(&m_crit); }

public:
    void lock() { ::EnterCriticalSection(&m_crit); }
    void unlock() { ::LeaveCriticalSection(&m_crit); }

    BOOL try_lock() { return ::TryEnterCriticalSection(&m_crit); }

private:
    CRITICAL_SECTION m_crit; // 临界区
};

#endif // MUTEX_HPP