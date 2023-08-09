//
// Created by Sungjun Park on 2022/12/18.
//
#include "HTTPDefinitions.hpp"
#include <cstring>

HTTP::AvailableMethods HTTP::methodStringToEnum(const std::string& method)
{
  for (size_t idx = 0; idx < sizeof(HTTP::AVAILABLE_METHODS); ++idx)
  {
    if (::strcmp(HTTP::AVAILABLE_METHODS[idx], method.c_str()) == 0)
      return (HTTP::AvailableMethods(idx));
  }
  return (HTTP::UNKNOWN);
}

std::string HTTP::methodTypeToString(HTTP::AvailableMethods method)
{
  switch (method)
  {
    case HTTP::GET:
    {
      return ("GET");
    }
    case HTTP::HEAD:
    {
      return ("HEAD");
    }
    case HTTP::POST:
    {
      return ("POST");
    }
    case HTTP::PUT:
    {
      return ("PUT");
    }
    case HTTP::DELETE:
    {
      return ("DELETE");
    }
    case HTTP::UNKNOWN:
    {
      return ("UNKNOWN");
    }
  }
}
