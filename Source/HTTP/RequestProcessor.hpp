//
// Created by Sungjun Park on 2022/12/18.
//

#ifndef REQUESTPROCESSOR_HPP
#define REQUESTPROCESSOR_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "VirtualServer.hpp"
#include "Server.hpp"
// just read request and make response

extern WS::Server* G_SERVER;

namespace HTTP
{
  class RequestProcessor
  {
  private:
      static HTTP::StatusCode checkError(HTTP::Request* request);
      static StatusCode checkHeader(HTTP::Request* request, WS::VirtualServer& server);
      static HTTP::StatusCode checkNoBodyMethod(HTTP::Request* request);
      static void setResponseLine(HTTP::Response* response, HTTP::StatusCode statusCode);
  public:
      static StatusCode checkRequest(HTTP::Request* request, WS::Connection* connection);
      static HTTP::Response* createResponse(HTTP::Request* request, WS::Connection* connection);
  };
}

#endif //REQUESTPROCESSOR_HPP
