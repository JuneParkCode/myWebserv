#ifndef RESPONSE_HPP
#define RESPONSE_HPP

//#include "../../Source/HTTP/HTTPDefinitions.hpp"
#include "Storage.hpp"
#include <string>
#include <map>

namespace HTTP
{
  class Response
  {
  private:
      std::string m_version;
      std::string m_statusCode;
      std::string m_statusMessage;
      std::map<std::string, std::string> m_headers;
      FileDescriptor m_readFD;
      FileDescriptor m_writeFD;
      bool isDone;
  public:
      void setHeader(const std::string& key, const std::string& value);
      void setStartLine(const std::string& version, const std::string& statusCode, const std::string& statusMessage);
      const std::string& getVersion() const;
      const std::string& getStatusCode() const;
      const std::string& getStatusMesseage() const;
      const std::string& getHeader(const std::string& key) const;
      void send();
      Response();
      ~Response();
  };
}

#endif //RESPONSE_HPP
