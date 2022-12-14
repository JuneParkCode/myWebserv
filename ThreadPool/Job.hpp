//
// Created by Sungjun Park on 2022/12/15.
//

#ifndef JOB_HPP
#define JOB_HPP

namespace WS
{
  typedef struct Job
  {
      void (*handler)(void*);
      void* data;
  } Job;
}

#endif //JOB_HPP
