/**
 * @file thread_win.h
 * @brief C++98 windows线程封装类，添加function新特性支持
 * @author zhengw
 * @date 2024-07-03
 */

#ifndef THREAD_WIN_H
#define THREAD_WIN_H

#include <process.h>
#include <windows.h>

#include <boost/function.hpp>

namespace this_thread
{
inline DWORD get_id() { return ::GetCurrentThreadId(); }
} // namespace this_thread

class thread
{
    typedef boost::function<void()> thread_func;

public:
    thread(const thread_func& v_func, BOOL v_bSuspend = FALSE)
        : m_hThread(NULL), m_uiThreadId(0), m_iPriority(THREAD_PRIORITY_NORMAL), m_bRunning(FALSE), m_func(v_func)
    {
        if (create() && !v_bSuspend)
        {
            start();
        }
    }
    virtual ~thread() { join_for(0); }

public:
    /**
     * @brief 创建线程
     */
    BOOL create()
    {
        if (valid())
        {
            return FALSE;
        }

        m_hThread = (HANDLE)::_beginthreadex(NULL, 0, run, this, CREATE_SUSPENDED, &m_uiThreadId);

        return valid();
    }

    /**
     * @brief 启动线程
     */
    void start()
    {
        if (valid() && !running())
        {
            ::ResumeThread(m_hThread);
            m_bRunning = TRUE;
        }
    }

    /**
     * @brief 等待线程结束
     */
    void join() { join_for(INFINITE); }

    /**
     * @brief 等待线程结束，超时直接终止线程
     * @param [in] v_dwTimeout 超时时间，单位毫秒
     */
    void join_for(DWORD v_dwTimeout)
    {
        if (joinable())
        {
            if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThread, v_dwTimeout))
            {
                terminate();
            }
        }
        detach();
    }

    /**
     * @brief 暂停线程
     */
    void interrupt()
    {
        if (joinable())
        {
            ::SuspendThread(m_hThread);
            m_bRunning = FALSE;
        }
    }

    /**
     * @brief 睡眠线程
     * @param [in] v_dwMilliseconds 睡眠时间，单位毫秒
     */
    void sleep_for(DWORD v_dwMilliseconds)
    {
        interrupt();
        ::Sleep(v_dwMilliseconds);
        start();
    }

    /**
     * @brief 分离线程
     */
    void detach()
    {
        if (valid())
        {
            ::CloseHandle(m_hThread);
            m_hThread = NULL;
            m_uiThreadId = 0;
            m_bRunning = FALSE;
        }
    }

    /**
     * @brief 终止线程
     */
    void terminate()
    {
        if (valid())
        {
            ::SuspendThread(m_hThread);
            ::TerminateThread(m_hThread, 0);
            detach();
        }
    }

    /**
     * @brief 设置线程优先级
     * @param [in] v_iPriority 优先级
     */
    void set_priority(int v_iPriority)
    {
        if (valid())
        {
            m_iPriority = v_iPriority;
            ::SetThreadPriority(m_hThread, m_iPriority);
        }
    }

    BOOL joinable() const { return (valid() && m_bRunning); }
    BOOL valid() const { return (m_hThread && m_hThread != INVALID_HANDLE_VALUE); }
    BOOL running() const { return m_bRunning; }
    UINT32 id() const { return m_uiThreadId; }
    int priority() const { return m_iPriority; }

    static DWORD hardware_concurrency()
    {
        SYSTEM_INFO sys_info;
        ::GetSystemInfo(&sys_info);
        return sys_info.dwNumberOfProcessors;
    }

private:
    /**
     * @brief 线程函数
     * @param [in] v_lpParam 线程参数，指向线程对象
     * @return
     */
    static UINT WINAPI run(LPVOID v_lpParam)
    {
        thread* pThread = static_cast<thread*>(v_lpParam);
        if (pThread)
        {
            if (pThread->m_func)
            {
                pThread->m_func();
            }
            pThread->m_bRunning = FALSE;
        }

        _endthreadex(0);
        return 0;
    }

private:
    HANDLE m_hThread;    // 线程句柄
    UINT32 m_uiThreadId; // 线程ID
    int m_iPriority;     // 线程优先级
    BOOL m_bRunning;     // 线程是否正在运行

    thread_func m_func; // 线程函数
};

#endif // THREAD_WIN_H