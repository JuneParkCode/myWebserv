//
// Created by Sungjun Park on 2022/12/15.
//

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Request.hpp"

namespace HTTP
{
  enum ParseStatus
  {
      REQ_PARSE_START,
      REQ_PARSE_HEADER,
      REQ_PARSE_BODY,
      REQ_PARSE_END,
      REQ_PARSE_ERROR
  };
  class RequestParser
  {
  private:
      ParseStatus m_parseStatus;
      HTTP::Request* m_request;
  private:
      void parseStartLine(WS::Storage& storage);
      void parseHeader(WS::Storage& storage);
      void parseBody(WS::Storage& storage);
      void parseChunkedBody(WS::Storage& storage);
  public:
      HTTP::Request* parse(WS::Storage& storage, bool isForce);
  };
}

#endif //REQUESTPARSER_HPP
