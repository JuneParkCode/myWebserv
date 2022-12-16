//
// Created by Sungjun Park on 2022/12/15.
//

#ifndef AREQUEST_HPP
#define AREQUEST_HPP

namespace WS
{
  class ARequest
  {
  public:
      ARequest() = default;
      virtual ~ARequest() = default;
      virtual void response() = 0;
  };
}

#endif //AREQUEST_HPP
