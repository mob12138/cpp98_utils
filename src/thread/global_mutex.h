#ifndef GLOBAL_MUTEX_H
#define GLOBAL_MUTEX_H

// 封装windows api，实现全局互斥锁

#include <windows.h>

class global_mutex
{

private:
    HANDLE m_mutex;
};

#endif