#include "ThreadPool.h"

ThreadPool::ThreadPool(int number)
    :m_iThreadNumber(number)
{
}

ThreadPool::~ThreadPool()
{
    stopPool();
}

void ThreadPool::startPool()
{
    m_bRunning = true;
    for (int i = 0; i < m_iThreadNumber; ++i)
    {
        std::thread t(&ThreadPool::doTask, this);
        m_vecThread.push_back(std::move(t));
    }
}

void ThreadPool::stopPool()
{
    //运行状态置为false
    m_bRunning = false;
    //逻辑和析构函数一样
    m_dequeNotEmptyCV.notify_all();
    for (auto& item : m_vecThread)
    {
        //回收线程资源
        if (item.joinable())
        {
            item.join();
        }
    }
}

void ThreadPool::doTask()
{
    while (1)
    {
        //要获取的任务
        ThreadTask task;
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            //正在运行状态且deque大小为空
            /*if (m_bIsRunning && 0 == m_deqTask.size())
            {
                printf("thread id:%d,", GetCurrentThreadId());
                printf("wiat state\n");
                //没有任务的时候就等待
                m_conDequeNotEmpty.wait(lck);
            }*/
            //等待任务队列不空或者状态为退出
            m_dequeNotEmptyCV.wait(lck, [this]() {return !m_bRunning || !m_dequeTask.empty(); });
            //不在运行状态且任务队列空了那就退出
            if (!m_bRunning && m_dequeTask.empty())
            {
                return;
            }
            //有任务就把顶端的拿出来
            task = std::move(m_dequeTask.front());
            m_dequeTask.pop_front();
        }
        //出了上边的作用域,lck就自动解锁了
        //执行任务
        task();
    }
}

