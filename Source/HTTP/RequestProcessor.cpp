//
// Created by Sungjun Park on 2022/12/18.
//

#include "RequestProcessor.hpp"
#include <iostream>

HTTP::Response* HTTP::RequestProcessor::createResponse(HTTP::Request* request, WS::Connection* connection)
{
  auto response = new HTTP::Response(request, connection);
  auto status = checkRequest(request, connection);

  setResponseLine(response, status);
  // FIXME: set connection
  auto& headers = request->getHeaders();
  auto connection_header = headers.find("Connection");
  if (connection_header == headers.end() || connection_header->second == "close")
  {
    connection->m_closed = true;
    response->setHeader("Connection", "close");
  }
  else
  {
    response->setHeader("Connection", "keep-alive");
  }
  return (response);
}

HTTP::StatusCode HTTP::RequestProcessor::checkNoBodyMethod(HTTP::Request* request)
{
  if ((request->getContentLength() > 0) && (request->getMethod() == "GET" || request->getMethod() == "HEAD"))
    return (HTTP::ST_BAD_REQUEST);
  // check chunked
  const auto& headers = request->getHeaders();
  const auto& it = headers.find("Transfer-Encoding");
  if (it != headers.cend() && it->second.find("chunked") != std::string::npos)
    return (HTTP::ST_BAD_REQUEST);
  return HTTP::ST_OK;
}

HTTP::StatusCode HTTP::RequestProcessor::checkError(HTTP::Request* request)
{
  if (request->isErrorRequest())
    return (HTTP::StatusCode::ST_BAD_REQUEST);
  return (HTTP::StatusCode::ST_OK);
}

HTTP::StatusCode HTTP::RequestProcessor::checkHeader(HTTP::Request* request, WS::VirtualServer& server)
{
  HTTP::StatusCode status;

  if ((status = checkNoBodyMethod(request)) >= 400)
    return (status);
  return (server.checkRequestHeader(request));
}

HTTP::StatusCode HTTP::RequestProcessor::checkRequest(HTTP::Request* request, WS::Connection* connection)
{
  HTTP::StatusCode statusCode;
  auto& server = connection->getServer();

  if ((statusCode = checkError(request)) >= 400)
    return (statusCode);
  if ((statusCode = checkHeader(request, server)) >= 400)
    return (statusCode);
  return (statusCode);
}

void HTTP::RequestProcessor::setResponseLine(HTTP::Response* response, HTTP::StatusCode statusCode)
{
  switch (statusCode)
  {
    case ST_OK:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "OK");
      break;
    }
    case ST_CONTINUE:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "CONTINUE");
      break;
    }
    case ST_SWITCHING_PROTOCOLS:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "SWITCHING_PROTOCOLS");
      break;
    }
    case ST_CREATED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "CREATED");
      break;
    }
    case ST_ACCEPTED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "ACCEPTED");
      break;
    }
    case ST_NON_AUTHORITATIVE_INFORMATION:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "NON_AUTHORITATIVE_INFORMATION");
      break;
    }
    case ST_NO_CONTENT:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "NO_CONTENT");
      break;
    }
    case ST_RESET_CONTENT:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "RESET_CONTENT");
      break;
    }
    case ST_PARTIAL_CONTENT:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "PARTIAL_CONTENT");
      break;
    }
    case ST_MULTIPLE_CHOICES:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "MULTIPLE_CHOICES");
      break;
    }
    case ST_MOVED_PERMANENTLY:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "MOVED_PERMANENTLY");
      break;
    }
    case ST_FOUND:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "FOUND");
      break;
    }
    case ST_SEE_OTHER:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "SEE_OTHER");
      break;
    }
    case ST_NOT_MODIFIED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "NOT_MODIFIED");
      break;
    }
    case ST_TEMPORARY_REDIRECT:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "TEMPORARY_REDIRECT");
      break;
    }
    case ST_PERMANENT_REDIRECT:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "PERMANENT_REDIRECT");
      break;
    }
    case ST_BAD_REQUEST:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "BAD_REQUEST");
      break;
    }
    case ST_UNAUTHORIZED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "UNAUTHORIZED");
      break;
    }
    case ST_FORBIDDEN:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "FORBIDDEN");
      break;
    }
    case ST_NOT_FOUND:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "NOT_FOUND");
      break;
    }
    case ST_METHOD_NOT_ALLOWED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "METHOD_NOT_ALLOWED");
      break;
    }
    case ST_REQUEST_TIMEOUT:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "REQUEST_TIMEOUT");
      break;
    }
    case ST_LENGTH_REQUIRED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "LENGTH_REQUIRED");
      break;
    }
    case ST_PAYLOAD_TOO_LARGE:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "PAYLOAD_TOO_LARGE");
      break;
    }
    case ST_INTERNAL_SERVER_ERROR:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "INTERNAL_SERVER_ERROR");
      break;
    }
    case ST_NOT_IMPLEMENTED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "NOT_IMPLEMENTED");
      break;
    }
    case ST_BAD_GATEWAY:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "BAD_GATEWAY");
      break;
    }
    case ST_SERVICE_UNAVAILABLE:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "SERVICE_UNAVAILABLE");
      break;
    }
    case ST_GATEWAY_TIMEOUT:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "GATEWAY_TIMEOUT");
      break;
    }
    case ST_HTTP_VERSION_NOT_SUPPORTED:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(statusCode), "HTTP_VERSION_NOT_SUPPORTED");
      break;
    }
    case ST_ERROR:
    {
      response->setResponseLine("HTTP/1.1", std::to_string(ST_SERVICE_UNAVAILABLE), "SERVICE_UNAVAILABLE");
      break;
    }
  }
}