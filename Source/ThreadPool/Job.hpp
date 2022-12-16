//
// Created by Sungjun Park on 2022/12/15.
//

#ifndef JOB_HPP
#define JOB_HPP

#include <functional>
#include <utility>

namespace WS
{
  typedef struct Job
  {
     std::function<void()> handler;
     explicit Job(std::function<void()> handler_) : handler(std::move(handler_)){};
  } Job;
}

#endif //JOB_HPP
