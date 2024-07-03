/**
 * @file message_queue.h
 * @brief 消息队列，用于线程间通信
 * @author zhengw
 * @date 2024-06-09
 */

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <deque>
#include "mutex.h"
#include "condition_variable.h"

/**
 * @brief 消息队列，用于线程间通信
 * @tparam T 消息类型
 */
template <typename T> class message_queue
{
    typedef std::deque<T> deque_;
    typedef unique_lock<mutex> unique_lock_;
    typedef unique_lock<mutex> lock_guard_;

public:
    /**
     * @brief 消息队列构造函数
     * @param [in] v_nCapacity 队列容量，0表示无限容量
     */
    message_queue(size_t v_nCapacity = 10000) : m_nCapacity(v_nCapacity) {}
    virtual ~message_queue() {}

public:
    /**
     * @brief 向队列尾部添加消息
     * @param [in] v_tMsg 消息内容
     */
    void push_back(const T& v_tMsg)
    {
        unique_lock_ lock(m_mutexPut);
        while (put_deque_full())
        {
            m_cvGet.notify_one();
            m_cvPut.wait(lock);
        }

        m_dequePut.push_back(v_tMsg);
        m_cvGet.notify_one();
    }

    /**
     * @brief 尝试向队列尾部添加消息
     * @param [in] v_tMsg 消息内容
     * @return BOOL 是否成功添加
     */
    BOOL try_push_back(const T& v_tMsg)
    {
        if (m_mutexPut.try_lock())
        {
            if (put_deque_full())
            {
                m_cvGet.notify_one();
                m_mutexPut.unlock();
                return FALSE;
            }

            m_dequePut.push_back(v_tMsg);

            m_cvGet.notify_one();
            m_mutexPut.unlock();
            return TRUE;
        }

        return FALSE;
    }

    /**
     * @brief 向队列头部添加消息
     * @param [in] v_tMsg 消息内容
     */
    void push_front(const T& v_tMsg)
    {
        unique_lock_ lock(m_mutexGet);
        while (get_deque_full())
        {
            m_cvGet.notify_one();
            m_cvPut.wait(lock);
        }

        m_dequeGet.push_front(v_tMsg);
        m_cvGet.notify_one();
    }

    /**
     * @brief 将队列头部消息弹出
     * @note 若队列为空，则等待直到队列不为空；
     * @param [out] v_tMsg 弹出的消息内容
     */
    void pop(T& v_tMsg)
    {
        unique_lock_ lockGet(m_mutexGet);
        if (m_dequeGet.empty())
        {
            unique_lock_ lockPut(m_mutexPut);

            unsigned int nTimeOuts = 0;
            while (all_deque_empty())
            {
                // 避免死锁，设置超时时间
                if (nTimeOuts < 1000)
                {
                    nTimeOuts += 50;
                }

                m_cvPut.notify_all();

                lockPut.unlock();
                m_cvGet.wait_for(lockGet, nTimeOuts);
                lockPut.lock();
            }

            if (m_dequeGet.empty())
            {
                m_dequeGet.swap(m_dequePut);
            }
        }

        v_tMsg = m_dequeGet.front();
        m_dequeGet.pop_front();

        if (all_deque_empty())
        {
            m_cvPut.notify_all();
        }
    }

    /**
     * @brief 尝试将队列头部消息弹出
     * @param [out] v_tMsg 弹出的消息内容
     * @return BOOL 是否成功弹出
     */
    BOOL try_pop(T& v_tMsg)
    {
        if (m_mutexGet.try_lock())
        {
            if (m_dequeGet.empty() && !try_swap_deque())
            {
                m_mutexGet.unlock();
                return FALSE;
            }

            v_tMsg = m_dequeGet.front();
            m_dequeGet.pop_front();

            m_mutexGet.unlock();
            return TRUE;
        }

        return FALSE;
    }

public:
    size_t size()
    {
        lock_guard_ lockGet(m_mutexGet);
        lock_guard_ lockPut(m_mutexPut);
        return m_dequeGet.size() + m_dequePut.size();
    }
    BOOL empty()
    {
        lock_guard_ lockGet(m_mutexGet);
        lock_guard_ lockPut(m_mutexPut);
        return m_dequeGet.empty() && m_dequePut.empty();
    }
    size_t capacity() const { return m_nCapacity; }
    void set_capacity(size_t v_nCapacity)
    {
        lock_guard_ lockGet(m_mutexGet);
        lock_guard_ lockPut(m_mutexPut);
        m_nCapacity = v_nCapacity;
    }
    void clear()
    {
        lock_guard_ lockGet(m_mutexGet);
        lock_guard_ lockPut(m_mutexPut);
        m_dequeGet.clear();
        m_dequePut.clear();
    }

private:
    /**
     * @brief 尝试交换生产者队列和消费者队列
     * @note 需在锁定状态下调用
     * @return BOOL 是否成功交换
     */
    BOOL try_swap_deque()
    {
        if (m_mutexPut.try_lock())
        {
            if (m_dequePut.empty())
            {
                m_mutexPut.unlock();
                return FALSE;
            }

            m_dequeGet.swap(m_dequePut);
            m_cvPut.notify_all();

            m_mutexPut.unlock();
            return TRUE;
        }

        return FALSE;
    }

    /**
     * @brief 判断生产者队列是否已满
     * @note 非线程安全，需在锁定状态下调用
     * @return BOOL 是否已满
     */
    BOOL put_deque_full() const { return m_nCapacity > 0 && m_dequePut.size() >= m_nCapacity; }

    /**
     * @brief 判断消费者队列是否已满
     * @note 非线程安全，需在锁定状态下调用
     * @return BOOL 是否已满
     */
    BOOL get_deque_full() const { return m_nCapacity > 0 && m_dequeGet.size() >= m_nCapacity; }

    /**
     * @brief 判断队列是否已空
     * @note 非线程安全，需在锁定状态下调用
     * @return BOOL 是否已空
     */
    BOOL all_deque_empty() const { return m_dequeGet.empty() && m_dequePut.empty(); }

private:
    deque_ m_dequeGet;          // 消费者队列
    mutex m_mutexGet;           // 消费者队列锁
    condition_variable m_cvGet; // 消费者条件变量

    deque_ m_dequePut;          // 生产者队列
    mutex m_mutexPut;           // 生产者队列锁
    condition_variable m_cvPut; // 生产者条件变量

    size_t m_nCapacity; // 队列容量
};

#endif // MESSAGE_QUEUE_H