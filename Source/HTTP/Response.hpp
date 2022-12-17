#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "AResponse.hpp"
#include "Request.hpp"
#include "Connection.hpp"
#include "HTTPDefinitions.hpp"
#include "Storage.hpp"
#include <string>
#include <unordered_map>

namespace HTTP
{
  class Response : public WS::AResponse
  {
  private:
      std::string m_version;
      std::string m_statusCode;
      std::string m_statusMessage;
      std::unordered_map<std::string, std::string> m_headers;
      HTTP::Request* m_request;
      WS::Connection* m_connection;
      bool m_isDone;
  public:
      void setHeader(const std::string& key, const std::string& value);
      void setResponseLine(const std::string& version, const std::string& statusCode, const std::string& statusMessage);
      const std::string& getVersion() const;
      const std::string& getStatusCode() const;
      const std::string& getStatusMesseage() const;
      const std::unordered_map<std::string, std::string>& getHeader() const;
      void send() override; // using socketSendBuffer in connection...
      Response(HTTP::Request* request, WS::Connection* connection);
      ~Response() override = default;
  };
}

#endif //RESPONSE_HPP
