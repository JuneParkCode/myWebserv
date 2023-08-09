//
// Created by Sungjun Park on 2022/12/13.
//

#include "Router.hpp"

void WS::Router::addMethodProc(const std::string& method, std::function<void(HTTP::Request*, HTTP::Response*)> proc)
{
  m_methodProc[method] = proc;
}

std::function<void(HTTP::Request*, HTTP::Response*)> WS::Router::getMethodProc(const std::string& method)
{
  std::function<void(HTTP::Request*, HTTP::Response*)> lambda = ([](HTTP::Request* req, HTTP::Response* res){
    res->setResponseLine("HTTP/1.1", "405", "NOT_ALLOWED_METHOD");
    res->sendFile("../webserv.cpp");
  });
  if (!isAllowedMethod(method))
    return (lambda);
  return (m_methodProc[method]);
}

bool WS::Router::isAllowedMethod(const std::string& method) const
{
  auto it = m_methodProc.find(method);

  return (it != m_methodProc.end());
}

WS::Router::Router(const std::string& path):
  m_path(path)
{
}
