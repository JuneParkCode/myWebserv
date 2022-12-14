//
// Created by Sungjun Park on 2022/12/11.
//

#include "ThreadPool.hpp"
#include "Handlers.hpp"

void WS::ThreadPool::enqueueIOJob(const struct kevent& ev)
{
  if (stop)
    throw (std::runtime_error("thread stopped\n"));
  std::lock_guard<std::mutex> lock(m_ioJobQueueMutex);
  m_ioJobQueue.push(ev);
  m_cvIOJobQueue.notify_one();
}

void WS::ThreadPool::enqueueNormalJob(const Job& job)
{
  if (stop)
    throw (std::runtime_error("thread stopped\n"));
  std::lock_guard<std::mutex> lock(m_normalJobQueueMutex);
  m_normalJobQueue.push(job);
  m_cvNormalJobQueue.notify_one();
}

WS::ThreadPool::~ThreadPool()
{
  stop = true;
  m_cvIOJobQueue.notify_all();
  for (auto& thread : m_threads)
  {
    thread.join();
  }
}

// I/O 작업을 위한 thread와 일반 논리적 작업을 위한 thread를 구분시켜 생성함.
WS::ThreadPool::ThreadPool(const size_t numThreads) :
        NUM_THREADS(numThreads), stop(false)
{
  const size_t NUM_IO_THREADS = NUM_THREADS / 2;
  const size_t NUM_NORMAL_THREADS = NUM_THREADS - NUM_IO_THREADS;

  m_threads.reserve(NUM_THREADS);
  for (size_t i = 0; i < NUM_IO_THREADS; ++i)
  {
    m_threads.emplace_back([this](){this->workIOJob();});
  }
  for (size_t i = 0; i < NUM_NORMAL_THREADS; ++i)
  {
    m_threads.emplace_back([this](){this->workNormalJob();});
  }
}

void WS::ThreadPool::workIOJob()
{
  auto waitCondition = [this](){
    return (this->stop || !this->m_ioJobQueue.empty());
  };

  while (true)
  {
    std::unique_lock<std::mutex> lock(m_ioJobQueueMutex);

    m_cvIOJobQueue.wait(lock, waitCondition);
    if (stop)
      return ;
    else if (!this->m_ioJobQueue.empty())
    {
      auto newEV = m_ioJobQueue.front();
      m_ioJobQueue.pop();
      lock.unlock();
      WS::handleEvent(newEV);
    }
  }
}

void WS::ThreadPool::workNormalJob()
{
  auto waitCondition = [this](){
    return (this->stop || !this->m_normalJobQueue.empty());
  };

  while (true)
  {
    std::unique_lock<std::mutex> lock(m_normalJobQueueMutex);

    m_cvNormalJobQueue.wait(lock, waitCondition);
    if (stop)
      return ;
    else if (!this->m_normalJobQueue.empty())
    {
      auto newJob = m_normalJobQueue.front();
      m_normalJobQueue.pop();
      lock.unlock();
      newJob.handler(newJob.data);
    }
  }
}
