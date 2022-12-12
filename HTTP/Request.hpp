#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Storage.hpp"
#include "HTTPDefinitions.hpp"
#include "Response.hpp"
#include <map>
#include <iostream>
#include <chrono>

namespace HTTP
{
  class Request
  {
  private:
      std::string m_method;
      std::string m_URL;
      std::string m_version;
      std::map<std::string, std::string> m_headers;
      WS::Storage m_body;
      ssize_t m_contentLength;
      std::chrono::system_clock m_requestTime;
      FileDescriptor m_connectedSocketFD;
      FileDescriptor m_readFD;
      FileDescriptor m_writeFD;
      HTTP::Response m_response;
  public:
      const std::string& getHeader(const std::string& key) const;
      const WS::Storage& getBody() const;
      const std::string& getMethod() const;
      const std::string& getURL() const;
      const std::string& getVersion() const;
      HTTP::Response& response();
      bool isTimeout();
      bool isFinished();
      Request();
      ~Request();
  };
}

std::ostream& operator<<(std::ostream& stream, const HTTP::Request& req);


#endif //REQUEST_HPP
