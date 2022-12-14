//
// Created by Sungjun Park on 2022/12/11.
//

#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include "Event.hpp"
#include "Job.hpp"
#include <thread>
#include <queue>
#include <vector>
#include <condition_variable>
#include <sys/event.h>

#define NUM_THREADS_DEF 12

namespace WS
{
  class ThreadPool
  {
  private:
      const size_t NUM_THREADS;
      bool stop;
      std::vector<std::thread> m_threads;
      std::queue<struct kevent> m_ioJobQueue;
      std::queue<Job> m_normalJobQueue;
      std::mutex m_ioJobQueueMutex;
      std::mutex m_normalJobQueueMutex;
      std::condition_variable m_cvIOJobQueue;
      std::condition_variable m_cvNormalJobQueue;
  public:
      void workIOJob();
      void workNormalJob();
      void enqueueIOJob(const struct kevent& ev);
      void enqueueNormalJob(const Job& job);
      explicit ThreadPool(size_t numThreads);
      ~ThreadPool();
  };
}



#endif //THREADPOOL_HPP
