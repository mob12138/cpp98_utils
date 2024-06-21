#ifndef SHARED_MUTEX_H
#define SHARED_MUTEX_H

#include "condition_variable.h"
#include "mutex.h"

/**
 * @brief 局部共享锁的模板实现
 */
template <class T>
class shared_lock
{
public:
    shared_lock(T& mutex) : m_mutex(mutex), m_bLocked(FALSE) { lock_shared(); }
    ~shared_lock() { unlock_shared(); }

    void lock_shared()
    {
        if (!m_bLocked)
        {
            m_mutex.lock_shared();
            m_bLocked = TRUE;
        }
    }

    void unlock_shared()
    {
        if (m_bLocked)
        {
            m_mutex.unlock_shared();
            m_bLocked = FALSE;
        }
    }

private:
    T& m_mutex;
    BOOL m_bLocked;
};

/**
 * @brief 共享/读写互斥量
 */
class shared_mutex
{
public:
    shared_mutex() : m_nReaders(0), m_bWriting(FALSE) {}

public:
    void lock_shared()
    {
        unique_lock<mutex> lock(m_mutex);
        while (m_bWriting)
        {
            m_cvReaders.wait(lock);
        }

        ++m_nReaders;
    }
    void unlock_shared()
    {
        unique_lock<mutex> lock(m_mutex);
        if (--m_nReaders == 0 && m_bWriting)
        {
            m_cvWriters.notify_one();
        }
    }

    void lock()
    {
        unique_lock<mutex> lock(m_mutex);
        while (m_nReaders > 0 || m_bWriting)
        {
            m_cvWriters.wait(lock);
        }

        m_bWriting = TRUE;
    }
    void unlock()
    {
        unique_lock<mutex> lock(m_mutex);
        m_bWriting = FALSE;
        m_cvReaders.notify_all();
        m_cvWriters.notify_one();
    }

private:
    mutex m_mutex;                  // 互斥量
    condition_variable m_cvReaders; // 读者条件变量
    condition_variable m_cvWriters; // 写者条件变量
    UINT m_nReaders;                // 读者数量
    BOOL m_bWriting;                // 写者标志
};

#endif // SHARED_MUTEX_H