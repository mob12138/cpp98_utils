/**
 * @file win_event.h
 * @brief 封装 windows api 中的事件类
 * @author zhengw
 * @date 2024-06-07
 */

#ifndef WIN_EVENT_H
#define WIN_EVENT_H

#include <windows.h>

#include <vector>

/**
 * @brief windows api 中的事件类
 */
class win_event
{
public:
    /**
     * @brief 构造函数，创建事件
     * @param [in] v_lpEventAttributes 事件属性
     * @param [in] v_bManualReset
     * 是否手动重置，区别：自动重置事件会在第一次设置后自动清除，手动重置事件需要调用
     * reset() 手动清除
     * @param [in] v_bInitialState 初始状态
     * @param [in] v_lpName 事件名称
     */
    win_event(LPSECURITY_ATTRIBUTES v_lpEventAttributes = NULL, BOOL v_bManualReset = FALSE,
              BOOL v_bInitialState = FALSE, LPCWSTR v_lpName = NULL)
        : m_handle(::CreateEventW(v_lpEventAttributes, v_bManualReset, v_bInitialState, v_lpName))
    {
    }

    /**
     * @brief 构造函数，打开已存在的事件
     * @param [in] v_dwDesiredAccess 访问权限
     * @param [in] v_bInheritHandle 是否继承
     * @param [in] v_lpName 事件名称
     */
    win_event(DWORD v_dwDesiredAccess, BOOL v_bInheritHandle, LPCWSTR v_lpName)
        : m_handle(::OpenEventW(v_dwDesiredAccess, v_bInheritHandle, v_lpName))
    {
    }

    /**
     * @brief 构造函数，从句柄构造
     * @param [in] v_hEvent 事件句柄
     */
    win_event(HANDLE v_hEvent) : m_handle(v_hEvent) {}

    virtual ~win_event() { disconnect(); }

public:
    /**
     * @brief 等待事件，若超时则返回 FALSE
     * @param [in] v_dwMilliseconds 超时时间，单位为毫秒
     * @return (BOOL) TRUE 事件被设置，FALSE 超时或失败
     */
    BOOL wait_for(DWORD v_dwMilliseconds) const
    {
        return ::WaitForSingleObject(m_handle, v_dwMilliseconds) == WAIT_OBJECT_0;
    }

    /**
     * @brief 等待事件,无限等待
     */
    void wait() const { wait_for(INFINITE); }

    /**
     * @brief 是否已设置事件
     * @return (BOOL) TRUE 已设置，FALSE 未设置
     */
    BOOL signaled() const { return wait_for(0); }

    /**
     * @brief 设置事件
     */
    void set() const { ::SetEvent(m_handle); }

    /**
     * @brief 重置事件
     */
    void reset() const { ::ResetEvent(m_handle); }

    /**
     * @brief 句柄是否有效
     * @return (BOOL) TRUE 有效，FALSE 无效
     */
    BOOL valid() const { return m_handle && m_handle != INVALID_HANDLE_VALUE; }

    /**
     * @brief 关闭事件
     */
    void disconnect() const
    {
        if (valid())
        {
            ::CloseHandle(m_handle);
        }
    }

public:
    HANDLE handle() const { return m_handle; }

private:
    HANDLE m_handle;
};

/**
 * @brief 封装 windows api 中的事件集合类
 */
class win_events
{
    typedef std::vector<HANDLE> handles;

public:
    win_events() {}
    virtual ~win_events() {}

public:
    /**
     * @brief 添加事件
     * @note 非线程安全
     * @param [in] v_event 事件
     */
    void add(win_event& v_event) { m_events.push_back(v_event.handle()); }
    /**
     * @brief 移除事件
     * @note 非线程安全
     * @param [in] v_event 事件
     */
    void remove(win_event& v_event)
    {
        handles::iterator it = std::find(m_events.begin(), m_events.end(), v_event.handle());
        if (it != m_events.end())
        {
            m_events.erase(it);
        }
    }
    /**
     * @brief 等待添加的事件，若超时则返回 WAIT_TIMEOUT
     * @note 线程安全
     * @param [in] v_dwMilliseconds 超时时间，单位为毫秒
     * @param [in] v_bWaitAll 是否等待所有事件
     * @return DWORD
     * @retval WAIT_OBJECT_0+n 事件 n 被设置
     * @retval WAIT_TIMEOUT 超时
     * @retval WAIT_FAILED 失败
     */
    DWORD wait_for(DWORD v_dwMilliseconds, BOOL v_bWaitAll = FALSE)
    {
        if (m_events.empty())
        {
            return WAIT_FAILED;
        }

        return ::WaitForMultipleObjects(static_cast<DWORD>(m_events.size()), &m_events[0], v_bWaitAll,
                                        v_dwMilliseconds);
    }

    /**
     * @brief 判断wait_for的返回值是否为事件被设置
     * @param [in] v_dwResult wait_for的返回值
     * @return (BOOL) TRUE 事件被设置，FALSE 超时或失败
     */
    static BOOL is_wait_for_seted(DWORD v_dwResult) { return v_dwResult != WAIT_TIMEOUT && v_dwResult != WAIT_FAILED; }

    /**
     * @brief 通过wait_for返回的事件索引获取事件句柄
     * @param [in] v_dwResult 事件索引
     * @return HANDLE 事件句柄
     */
    HANDLE handle(DWORD v_dwResult) const
    {
        size_t n_index = static_cast<size_t>(v_dwResult - WAIT_OBJECT_0);
        if (n_index >= m_events.size())
        {
            return NULL;
        }
        return m_events[n_index];
    }

private:
    handles m_events;
};

#endif // WIN_EVENT_H
