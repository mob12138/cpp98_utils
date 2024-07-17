/**
 * @file unique_ptr.hpp
 * @brief C++98 实现智能指针unique_ptr
 * @author zhengw
 * @date 2024-07-18
 */

#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include <algorithm>
#include <cassert>
#define SHARED_ASSERT(x) assert(x)

/**
 * @brief 虚假的move实现，在 C++98 代替 C++11 std::move()
 */
template <typename T>
inline T &move(T &v)
{
    return v;
}

/**
 * @brief unique_ptr 的最小实现，是C++11 std::unique_ptr 和 boost::unique_ptr的子集
 * @details
 * unique_ptr 是一个智能指针，通过提供的指针保留对象的所有权
 * 没有 shared_ptr 引用计数器的开销
 * 它可以移动,但不允许共享此所有权
 * 当指针被销毁或重置时，销毁对象
 *
 * 此实现是虚假的，不能保证指针没有被复制，用于C++98
 */
template <class T>
class unique_ptr
{
public:
    typedef T element_type;

    unique_ptr(void) throw() : px(NULL) {}

    explicit unique_ptr(T *p) throw() : px(p) {}

    unique_ptr(const unique_ptr &ptr) throw() : px(ptr.px)
    {
        const_cast<unique_ptr &>(ptr).px = NULL;
    }

    unique_ptr &operator=(unique_ptr ptr) throw()
    {
        swap(ptr);
        return *this;
    }

    inline ~unique_ptr(void) throw() { destroy(); }

    inline void reset(void) throw() { destroy(); }

    void reset(T *p) throw()
    {
        SHARED_ASSERT((NULL == p) || (px != p));
        destroy();
        px = p;
    }

    void swap(unique_ptr &lhs) throw() { std::swap(px, lhs.px); }

    inline void release(void) throw() { px = NULL; }

    inline operator bool() const throw() { return (NULL != px); }

    inline T &operator*() const throw()
    {
        SHARED_ASSERT(NULL != px);
        return *px;
    }
    inline T *operator->() const throw()
    {
        SHARED_ASSERT(NULL != px);
        return px;
    }
    inline T *get(void) const throw() { return px; }

private:
    inline void destroy(void) throw()
    {
        delete px;
        px = NULL;
    }

    inline void release(void) const throw() { px = NULL; }

private:
    T *px;
};

template <class T, class U>
inline bool operator==(const unique_ptr<T> &l, const unique_ptr<U> &r) throw()
{
    return (l.get() == r.get());
}
template <class T, class U>
inline bool operator!=(const unique_ptr<T> &l, const unique_ptr<U> &r) throw()
{
    return (l.get() != r.get());
}
template <class T, class U>
inline bool operator<=(const unique_ptr<T> &l, const unique_ptr<U> &r) throw()
{
    return (l.get() <= r.get());
}
template <class T, class U>
inline bool operator<(const unique_ptr<T> &l, const unique_ptr<U> &r) throw()
{
    return (l.get() < r.get());
}
template <class T, class U>
inline bool operator>=(const unique_ptr<T> &l, const unique_ptr<U> &r) throw()
{
    return (l.get() >= r.get());
}
template <class T, class U>
inline bool operator>(const unique_ptr<T> &l, const unique_ptr<U> &r) throw()
{
    return (l.get() > r.get());
}

#endif // UNIQUE_PTR_HPP_INCLUDED