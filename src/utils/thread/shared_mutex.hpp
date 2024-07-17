#ifndef SHARED_MUTEX_HPP
#define SHARED_MUTEX_HPP

#include "condition_variable.hpp"
#include "mutex.hpp"

/**
 * @brief 局部共享锁的模板实现
 */
template <class T>
class shared_lock
{
public:
    shared_lock(T &mutex) : m_mutex(mutex), m_bLocked(FALSE) { lock_shared(); }
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
    T &m_mutex;
    BOOL m_bLocked;
};

/**
 * @brief 共享/读写互斥量
 */
class shared_mutex
{
public:
    shared_mutex() : m_bWriteEntered(false), m_nReaders(0) {}

public:
    void lock()
    {
        unique_lock<mutex> lock(m_mutex);
        while (m_bWriteEntered)
        {
            m_cvGate1.wait(lock);
        }

        m_bWriteEntered = true;

        while (m_nReaders > 0)
        {
            m_cvGate2.wait(lock);
        }
    }

    void unlock()
    {
        unique_lock<mutex> lock(m_mutex);
        m_bWriteEntered = false;
        m_cvGate1.notify_all();
    }

    bool try_lock()
    {
        unique_lock<mutex> lock(m_mutex);
        if (m_bWriteEntered || m_nReaders > 0)
        {
            return false;
        }
        m_bWriteEntered = true;
        return true;
    }

    void lock_shared()
    {
        unique_lock<mutex> lock(m_mutex);
        while (m_bWriteEntered)
        {
            m_cvGate1.wait(lock);
        }
        ++m_nReaders;
    }

    void unlock_shared()
    {
        unique_lock<mutex> lock(m_mutex);
        --m_nReaders;

        if (0 == m_nReaders && m_bWriteEntered)
        {
            m_cvGate2.notify_one();
        }
    }

    bool try_lock_shared()
    {
        unique_lock<mutex> lock(m_mutex);
        if (!m_bWriteEntered)
        {
            ++m_nReaders;
            return true;
        }
        return false;
    }

private:
    mutex m_mutex;
    condition_variable m_cvGate1;
    condition_variable m_cvGate2;
    bool m_bWriteEntered;
    unsigned int m_nReaders;
};

#endif // SHARED_MUTEX_HPP