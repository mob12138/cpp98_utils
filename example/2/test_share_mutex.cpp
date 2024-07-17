#include <iostream>
#include <thread>
#include <vector>
#include "../../src/utils/thread/shared_mutex.hpp"


int main()
{
    // 测试share_mutex
    shared_mutex sm;
    bool bRet = false;
    sm.lock();
    bRet = sm.try_lock_shared();
    if (bRet)
    {
        std::cout << "try_lock_shared success" << std::endl;
    }
    else
    {
        std::cout << "try_lock_shared failed" << std::endl;
    }
    sm.unlock();

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++)
    {
        threads.push_back(std::thread([&]() {
            bRet = sm.try_lock_shared();
            if (bRet)
            {
                std::cout << "try_lock_shared success" << std::endl;
            }
            else
            {
                std::cout << "try_lock_shared failed" << std::endl;
            }
        }));
    }

    for (auto& t : threads)
    {
        t.join();
    }

    bRet = sm.try_lock();
    if (bRet)
    {
        std::cout << "try_lock success" << std::endl;
    }
    else
    {
        std::cout << "try_lock failed" << std::endl;
    }

    for (auto& t : threads)
    {
        sm.unlock_shared();
    }
    bRet = sm.try_lock();
    if (bRet)
    {
        std::cout << "try_lock success" << std::endl;
    }
    else
    {
        std::cout << "try_lock failed" << std::endl;
    }
    return 0;
}