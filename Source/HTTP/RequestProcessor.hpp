//
// Created by Sungjun Park on 2022/12/18.
//

#ifndef REQUESTPROCESSOR_HPP
#define REQUESTPROCESSOR_HPP

#include "Request.hpp"
#include "Response.hpp"
// just read request and make response

namespace HTTP
{
  class RequestProcessor
  {
  private:
      static HTTP::StatusCode checkError(HTTP::Request* request);
      static HTTP::StatusCode checkVersion(HTTP::Request* request);
      static HTTP::StatusCode checkMethod(HTTP::Request* request);
      static HTTP::StatusCode checkHost(HTTP::Request* request);
      static HTTP::StatusCode checkPath(HTTP::Request* request);
      static HTTP::StatusCode checkHeader(HTTP::Request* request, WS::Connection* connection);
      static HTTP::StatusCode checkNoBodyMethod(HTTP::Request* request);
      static HTTP::StatusCode setResponseByMethod(HTTP::Response* response, HTTP::Request* request, WS::Connection* connection);
      static void setResponseLine(HTTP::Response* response, HTTP::StatusCode statusCode);

      static HTTP::StatusCode processGET(HTTP::Response* response, HTTP::Request* request, WS::Connection* connection);
      static HTTP::StatusCode processHEAD(HTTP::Response* response, HTTP::Request* request, WS::Connection* connection);
      static HTTP::StatusCode processPOST(HTTP::Response* response, HTTP::Request* request, WS::Connection* connection);
      static HTTP::StatusCode processPUT(HTTP::Response* response, HTTP::Request* request, WS::Connection* connection);
      static HTTP::StatusCode processDELETE(HTTP::Response* response, HTTP::Request* request, WS::Connection* connection);
  public:
      static HTTP::StatusCode checkPayload(HTTP::Request* request, size_t payload);
      static HTTP::StatusCode checkRequest(HTTP::Request* request, WS::Connection* connection);
      static HTTP::Response* createResponse(HTTP::Request* request, WS::Connection* connection);
  };
}

#endif //REQUESTPROCESSOR_HPP
