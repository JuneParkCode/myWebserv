//
// Created by Sungjun Park on 2022/12/16.
//

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "ARequest.hpp"
#include "Request.hpp"
#include "Storage.hpp"
#include <sys/event.h>

namespace WS
{
    class Connection;
}

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
      HTTP::Request* m_processingRequest;
      WS::Connection* m_connection;
      ParseStatus m_parseStatus;
      size_t m_chunkSize;
  private:
      void parseRequestHead(WS::Storage& buffer);
      void parseRequestLine(std::istringstream& stream);
      void parseHeader(std::istringstream& stream);
      void parseCommonBody(WS::Storage& buffer);
      void parseChunkedBody(WS::Storage& buffer);
      void parseRequestBody(WS::Storage& buffer);
      void init();
  public:
      Request* parse(struct kevent& event, WS::Storage& buffer);
      RequestParser(WS::Connection* connection);
      ~RequestParser();
  };
}

#endif //REQUESTPARSER_HPP
