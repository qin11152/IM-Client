#pragma once
#include "module/stdafx.h"

#include "boost/noncopyable.hpp"

using ThreadTask = std::function<void()>;

class ThreadPool :public boost::noncopyable
{
public:
    ThreadPool(int number);
    ~ThreadPool();

    void startPool();
    void stopPool();
    void doTask();

    template<typename F,typename ...Args>
    auto submit(F&& f,Args ...args)
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        ThreadTask task = [task_ptr]() {(*task_ptr)(); };
        m_dequeTask.push_back(task);
    }

private:

    std::deque<ThreadTask> m_dequeTask;         //�������Ķ���
    std::condition_variable m_dequeNotEmptyCV;  //deque�ǿ�֪ͨ
    std::vector<std::thread> m_vecThread;   //�̳߳��е��̱߳���
    std::mutex m_mutex;             //��
    int m_iThreadNumber{ 0 };       //�̳߳������߳�����
    bool m_bRunning{ false };       //�Ƿ�������

};


