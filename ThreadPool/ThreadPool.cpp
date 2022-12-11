//
// Created by Sungjun Park on 2022/12/11.
//

#include "ThreadPool.hpp"

void WS::ThreadPool::enqueueJob(const WS::Event& ev)
{
  if (stop)
    throw (std::runtime_error("thread stopped\n"));
  std::lock_guard<std::mutex> lock(m_jobQueueMutex);
  const size_t THREAD_NO = ev.connection->getThreadNO();
  if (THREAD_NO == 0)
  {
    m_jobQueue.push(ev);
    m_cvJobQueue.notify_all();
  }
  else
  {
    m_threadJobQueues[THREAD_NO - 1]->push(ev);
    m_cvJobQueue.notify_one();
  }
}

WS::ThreadPool::~ThreadPool()
{
  stop = true;
  m_cvJobQueue.notify_all();
  for (auto& thread : m_threads)
  {
    thread.join();
  }
}

WS::ThreadPool::ThreadPool(const size_t numThreads) :
        NUM_THREADS(numThreads), stop(false)
{
  m_threads.reserve(NUM_THREADS);
  for (size_t i = 0; i < NUM_THREADS; ++i)
  {
    m_threads.emplace_back([this, i](){this->work(i + 1);});
  }
}

void WS::ThreadPool::work(const size_t THREAD_NO)
{
  //set own queue
  std::queue<Event> threadJobQueue;
  std::unique_lock<std::mutex> tqLock(m_jobQueueMutex);
  this->m_threadJobQueues.push_back(&threadJobQueue);
  tqLock.unlock();

  while (true)
  {
    std::unique_lock<std::mutex> lock(m_jobQueueMutex);
    m_cvJobQueue.wait(lock, [this, &threadJobQueue](){return (this->stop || !this->m_jobQueue.empty() || !threadJobQueue.empty());});
    if (stop)
      return ;
    else if (!this->m_jobQueue.empty())
    {
      const Event& newEV = m_jobQueue.front();
      m_jobQueue.pop();
      lock.unlock();

      newEV.connection->setThreadNO(THREAD_NO);
      handleEvent(newEV);
    }
    else if (!threadJobQueue.empty())
    {
      const Event& newEV = threadJobQueue.front();
      threadJobQueue.pop();
      lock.unlock();
      
      handleEvent(newEV);
    }
  }
}
