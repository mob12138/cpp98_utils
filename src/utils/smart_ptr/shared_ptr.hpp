/**
 * @file shared_ptr.hpp
 * @brief C++98 实现智能指针shared_ptr
 * @author zhengw
 * @date 2024-07-18
 */

#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <algorithm>
#include <cassert>
#define SHARED_ASSERT(x) assert(x)

/**
 * @brief 存储智能指针引用计数
 */
class shared_ptr_count
{
public:
    shared_ptr_count() : pn(NULL) {}

    // 复制构造函数，复制引用计数
    shared_ptr_count(const shared_ptr_count &count) : pn(count.pn) {}

    // 实现拷贝并交换（拷贝构造函数和交换方法）
    void swap(shared_ptr_count &lhs) throw() { std::swap(pn, lhs.pn); }
    // 获取底层引用计数的getter方法
    long use_count(void) const throw()
    {
        long count = 0;
        if (NULL != pn)
        {
            count = *pn;
        }
        return count;
    }
    // 获得并共享指针的所有权，初始化引用计数
    template <class U>
    void acquire(U *p)
    {
        if (NULL != p)
        {
            if (NULL == pn)
            {
                try
                {
                    pn = new long(1); // 可能抛出 std::bad_alloc 异常
                }
                catch (std::bad_alloc &)
                {
                    delete p;
                    throw; // 再次抛出 std::bad_alloc 异常
                }
            }
            else
            {
                ++(*pn);
            }
        }
    }
    // 释放px指针的所有权，在适当的时候销毁对象
    template <class U>
    void release(U *p) throw()
    {
        if (NULL != pn)
        {
            --(*pn);
            if (0 == *pn)
            {
                delete p;
                delete pn;
            }
            pn = NULL;
        }
    }

public:
    long *pn; // 引用计数器
};

class shared_ptr_base
{
protected:
    shared_ptr_base(void) : pn() {}

    shared_ptr_base(const shared_ptr_base &other) : pn(other.pn) {}

    shared_ptr_count pn; // 引用计数器
};

/**
 * @brief 智能指针最小实现, C++11 std::shared_ptr or boost::shared_ptr的子集
 * @details
 * shared_ptr是一个智能指针，通过提供的指针保留对象的所有权，
 * 并与参考计数器共享此所有权。
 * 当指向该对象的最后一个共享指针被销毁或重置时，它会销毁该对象。
 */
template <class T>
class shared_ptr : public shared_ptr_base
{
public:
    // 托管对象的类型
    typedef T element_type;

    shared_ptr(void) throw() : shared_ptr_base(), px(NULL) {}

    explicit shared_ptr(T *p) : shared_ptr_base()
    {
        acquire(p); // 可能抛出 std::bad_alloc
    }

    // 构造函数共享所有权，仅用于pointer_cast（不管理两个单独的<T>和<U>指针）
    template <class U>
    shared_ptr(const shared_ptr<U> &ptr, T *p) : shared_ptr_base(ptr)
    {
        acquire(p); // 可能抛出 std::bad_alloc
    }

    // 复制构造函数以从另一种指针类型转换
    template <class U>
    shared_ptr(const shared_ptr<U> &ptr) throw() : shared_ptr_base(ptr)
    {
        SHARED_ASSERT((NULL == ptr.get()) || (0 != ptr.use_count()));            // 必须一致
        acquire(static_cast<typename shared_ptr<T>::element_type *>(ptr.get())); // 不会抛出 std::bad_alloc
    }

    // 复制构造函数
    shared_ptr(const shared_ptr &ptr) throw() : shared_ptr_base(ptr)
    {
        SHARED_ASSERT((NULL == ptr.px) || (0 != ptr.pn.use_count()));
        acquire(ptr.px); // 不会抛出 std::bad_alloc
    }

    // 赋值操作符
    shared_ptr &operator=(shared_ptr ptr) throw()
    {
        swap(ptr);
        return *this;
    }

    ~shared_ptr(void) throw() { release(); }

    // 释放所有权
    void reset(void) throw() { release(); }

    // 释放它的所有权，重新获得另一个所有权
    void reset(T *p)
    {
        SHARED_ASSERT((NULL == p) || (px != p)); // 不允许自动重置
        release();
        acquire(p);
    }

    // 交换两个共享指针
    void swap(shared_ptr &lhs) throw()
    {
        std::swap(px, lhs.px);
        pn.swap(lhs.pn);
    }

    // 判断是否有效
    operator bool() const throw() { return (0 < pn.use_count()); }
    // 判断是否唯一
    bool unique(void) const throw() { return (1 == pn.use_count()); }
    // 获取引用计数
    long use_count(void) const throw() { return pn.use_count(); }

    // 底层指针操作
    T &operator*() const throw()
    {
        SHARED_ASSERT(NULL != px);
        return *px;
    }
    T *operator->() const throw()
    {
        SHARED_ASSERT(NULL != px);
        return px;
    }
    T *get(void) const throw() { return px; }

private:
    // 获取/共享px指针的所有权，初始化引用计数器
    void acquire(T *p)
    {
        pn.acquire(p);
        px = p;
    }

    // 释放px指针的所有权，在适当的时候销毁对象
    void release(void) throw()
    {
        pn.release(px);
        px = NULL;
    }

private:
    T *px; // 托管指针
};

// 重载比较操作符
template <class T, class U>
bool operator==(const shared_ptr<T> &l, const shared_ptr<U> &r) throw()
{
    return (l.get() == r.get());
}
template <class T, class U>
bool operator!=(const shared_ptr<T> &l, const shared_ptr<U> &r) throw()
{
    return (l.get() != r.get());
}
template <class T, class U>
bool operator<=(const shared_ptr<T> &l, const shared_ptr<U> &r) throw()
{
    return (l.get() <= r.get());
}
template <class T, class U>
bool operator<(const shared_ptr<T> &l, const shared_ptr<U> &r) throw()
{
    return (l.get() < r.get());
}
template <class T, class U>
bool operator>=(const shared_ptr<T> &l, const shared_ptr<U> &r) throw()
{
    return (l.get() >= r.get());
}
template <class T, class U>
bool operator>(const shared_ptr<T> &l, const shared_ptr<U> &r) throw()
{
    return (l.get() > r.get());
}

// 静态类型转换
template <class T, class U>
shared_ptr<T> static_pointer_cast(const shared_ptr<U> &ptr)
{
    return shared_ptr<T>(ptr, static_cast<typename shared_ptr<T>::element_type *>(ptr.get()));
}

// 动态类型转换
template <class T, class U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U> &ptr)
{
    T *p = dynamic_cast<typename shared_ptr<T>::element_type *>(ptr.get());
    if (NULL != p)
    {
        return shared_ptr<T>(ptr, p);
    }
    else
    {
        return shared_ptr<T>();
    }
}

#endif // SHARED_PTR_HPP