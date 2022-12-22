//
// Created by Sungjun Park on 2022/12/12.
//

#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Event.hpp"
#include "Router.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <unordered_map>
#include <string>

#define BACKLOG 10240

typedef int FileDescriptor;

namespace WS
{
  class Server;
  class VirtualServer
  {
  private:
    const std::string m_listenIP;
    const std::string m_listenPort;
    std::string m_errorPagePath;
    std::unordered_map<std::string, Router> m_routers; // path;
    Event m_listenEvent;
    FileDescriptor m_serverFD;
    WS::Server* m_server;
    size_t m_payloadLimit;
  public:
    const std::string m_hostname;
  private:
    std::string findPath(const std::string& path) const;
  public:
    void listen();
    HTTP::StatusCode checkRequestHeader(HTTP::Request* request) const;
    void setPayloadLimit(size_t limit);
    void Get(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc);
    void Post(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc);
    void Put(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc);
    void Head(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc);
    void Delete(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc);
    void response(HTTP::Request* request, HTTP::Response* response);
    void setErrorPage(const char* errorPagePath);
    const char* getErrorPage() const;
  public:
    ~VirtualServer() = default;
    VirtualServer(const std::string& hostname, const std::string& IP, const std::string& port, WS::Server* server);
  };
}

#endif //VIRTUAL_SERVER_HPP
