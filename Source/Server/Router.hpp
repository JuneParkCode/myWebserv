//
// Created by Sungjun Park on 2022/12/13.
//

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HTTPDefinitions.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace WS
{
  class Router
  {
  private:
      std::string m_path;
      std::unordered_map<std::string, std::function<void(HTTP::Request*, HTTP::Response*)>> m_methodProc;
  public:
      void addMethodProc(const std::string& method, std::function<void(HTTP::Request*, HTTP::Response*)> proc);
      std::function<void(HTTP::Request*, HTTP::Response*)> getMethodProc(const std::string& method);
      bool isAllowedMethod(const std::string& method) const;
      explicit Router(const std::string& path);
      ~Router() = default;
  };
}

#endif //ROUTER_HPP
