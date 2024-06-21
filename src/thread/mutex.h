/**
 * @file mutex.h
 * @brief C++98 windows下的互斥量实现
 * @author zhengw
 * @date 2024-05-28
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <windows.h>
#include <string>

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

/**
 * @brief 信号量
 */
class semaphore
{
public:
    semaphore(LONG v_lMaximumCount, LONG v_lInitialCount = 0, wchar_t* v_szName = NULL) : m_strName(v_szName)
    {
        m_handle = ::CreateSemaphoreW(NULL, v_lInitialCount, v_lMaximumCount, v_szName);
    }

    ~semaphore() { ::CloseHandle(m_handle); }

    BOOL wait_for(DWORD v_dwMilliseconds) { return ::WaitForSingleObject(m_handle, v_dwMilliseconds) == WAIT_OBJECT_0; }

    void wait() { wait_for(INFINITE); }

    BOOL try_wait() { return wait_for(0); }

    void notify() { ::ReleaseSemaphore(m_handle, 1, NULL); }

    std::wstring name() const { return m_strName; }

private:
    HANDLE m_handle;
    std::wstring m_strName;
};

#endif // MUTEX_H