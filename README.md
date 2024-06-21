<!-- 下面是说明此项目的详细说明，具体内容为C++98的工具集 -->
# C++98 Toolkit
这是一个C++98的工具集，包含了一些常用的工具函数。
1. 封装了Windows下的文件操作。
2. 封装了Windows下的注册表操作。
3. 封装了Windows下的事件操作。
4. 仿照C++11的std::thread，封装了接口一样的线程类（基于Windows Api）。
5. 仿照C++11的std::mutex，封装了接口一样的互斥锁类（基于Windows Api）。
6. 仿照C++11的std::condition_variable，封装了接口一样的条件变量类（基于Windows Api）。
7. 封装了一个线程池。
8. 封装了一个线程安全的队列。
9. 引入了header-only的boost库，包含了boost::scoped_ptr、boost::shared_ptr、boost::weak_ptr三个智能指针；boost::bind用于绑定函数和函数对象；boost::function用于保存函数指针和函数对象；boost::variant用于保存任意类型的数据；boost::unordered_map、boost::unordered_set用于保存无序的键值对和集合。
