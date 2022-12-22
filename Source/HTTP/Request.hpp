//
// Created by Sungjun Park on 2022/12/16.
//

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "ARequest.hpp"
#include "Storage.hpp"
#include "HTTPDefinitions.hpp"
#include <string>
#include <unordered_map> // hash map
#include <ctime>

namespace HTTP
{
  class Response;
  class Request : public WS::ARequest
  {
  private:
      std::string m_method;
      std::string m_path;
      std::string m_version;
      std::unordered_map<std::string, std::string> m_headers;
      WS::Storage m_body;
      HTTP::Response* m_response;
      size_t m_contentLength;
      ::clock_t m_baseTime;
      bool m_isError;
  public:
      HTTP::StatusCode m_errorCode;
      // for test
      void display() const;
      WS::Storage& getBody();
      const std::unordered_map<std::string, std::string>& getHeaders() const;
      const std::string& getMethod() const;
      const std::string& getPath() const;
      const std::string& getVersion() const;
      size_t getContentLength() const;
      void setContentLength();
      void setContentLength(size_t len);
      void setError(HTTP::StatusCode statusCode);
      bool isErrorRequest() const;
      void setRequestLine(const std::string& method, const std::string& path, const std::string& version);
      void response() override;
      void setResponse(HTTP::Response* response);
      bool isChunked() const;
      void addHeader(const std::string& key, const std::string& value);
      double getThroughPut() const;
      Request() noexcept;
      ~Request();
  };
}

#endif //REQUEST_HPP
