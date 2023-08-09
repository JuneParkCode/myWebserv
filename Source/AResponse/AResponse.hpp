//
// Created by Sungjun Park on 2022/12/15.
//

#ifndef ARESPONSE_HPP
#define ARESPONSE_HPP

namespace WS
{
  class AResponse
  {
  public:
      AResponse() = default;
      virtual ~AResponse() = default;
      virtual void send() = 0;
  };
}

#endif //ARESPONSE_HPP
