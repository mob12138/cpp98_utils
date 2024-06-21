/**
 * @file atomic.h
 * @brief C++98 windows下的原子模板类实现
 * @author zhengw
 * @date 2024-06-07
 */

#ifndef ATOMIC_H
#define ATOMIC_H

/**
 * @brief 原子模板类，采用windows API实现原子操作，线程安全
 * @tparam T 原子操作的数据类型，只能是LONG，不支持LONG64是为了兼容XP系统
 */
template <typename T> class atomic
{
public:
    atomic(T v_value = 0) : m_value(v_value) {}

    // 取值
    operator T() const { return m_value; }

    // 重载++运算符
    atomic& operator++()
    {
        InterlockedIncrement(&m_value);
        return *this;
    }
    atomic operator++(int)
    {
        atomic temp = *this;
        InterlockedIncrement(&m_value);
        return temp;
    }

    // 重载--运算符
    atomic& operator--()
    {
        InterlockedDecrement(&m_value);
        return *this;
    }
    atomic operator--(int)
    {
        atomic temp = *this;
        InterlockedDecrement(&m_value);
        return temp;
    }

    // 重载+=运算符
    atomic& operator+=(T v_value)
    {
        InterlockedExchangeAdd(&m_value, v_value);
        return *this;
    }

    // 重载-=运算符
    atomic& operator-=(T v_value)
    {
        InterlockedExchangeAdd(&m_value, -v_value);
        return *this;
    }

    // 重载=运算符
    atomic& operator=(T v_value)
    {
        InterlockedExchange(&m_value, v_value);
        return *this;
    }

    // 重载==运算符
    bool operator==(T v_value) const { return m_value == v_value; }

    // 重载!=运算符
    bool operator!=(T v_value) const { return m_value != v_value; }

    // 重载<运算符
    bool operator<(T v_value) const { return m_value < v_value; }

    // 重载<=运算符
    bool operator<=(T v_value) const { return m_value <= v_value; }

    // 重载>运算符
    bool operator>(T v_value) const { return m_value > v_value; }

    // 重载>=运算符
    bool operator>=(T v_value) const { return m_value >= v_value; }

private:
    T m_value;
};

#endif // ATOMIC_H