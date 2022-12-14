#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Storage.hpp"
#include "HTTPDefinitions.hpp"
#include "Response.hpp"
#include "ARequest.hpp"
#include <map>
#include <iostream>
#include <chrono>

namespace HTTP
{
  class Request : public WS::ARequest
  {
  private:
      std::map<std::string, std::string> m_headers;
      WS::Storage m_body;
      ssize_t m_contentLength;
      std::chrono::system_clock m_requestTime; // for timeout
      HTTP::Response m_response;
      std::string m_method;
      std::string m_URL;
      std::string m_version;
  public:
      const std::string& getMethod() const;
      void setMethod(const std::string& mMethod);
      const std::string& getUrl() const;
      void setUrl(const std::string& mUrl);
      const std::string& getVersion() const;
      void setVersion(const std::string& mVersion);
      const std::map<std::string, std::string>& getHeaders() const;
      const WS::Storage& getBody() const;
      void setBody(const WS::Storage& mBody);
      ssize_t getContentLength() const;
      void setContentLength(ssize_t mContentLength);
      const std::chrono::system_clock& getRequestTime() const;
      void setRequestTime(const std::chrono::system_clock& mRequestTime);
      const Response& getResponse() const;
      void setResponse(const Response& mResponse);
  public:
      void response();
      bool isTimeout();
      bool isFinished();
      Request();
      ~Request() = default;
  };
}

std::ostream& operator<<(std::ostream& stream, const HTTP::Request& req);


#endif //REQUEST_HPP
