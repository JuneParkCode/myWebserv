//
// Created by Sungjun Park on 2022/12/12.
//

#include "VirtualServer.hpp"
#include "Server.hpp"
#include "Handlers.hpp"
#include <utility>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unordered_map>

WS::VirtualServer:: VirtualServer(const std::string& hostname, const std::string& IP, const std::string& port, WS::Server* server):
  m_hostname(hostname),
  m_listenIP(IP),
  m_listenPort(port),
  m_serverFD(-1),
  m_server(server),
  m_payloadLimit(SIZE_T_MAX) // NO_LIMIT
{
}

static struct sockaddr_in setSocketAddr(const std::string& IP, const std::string& PORT)
{
  struct sockaddr_in sockIN;

  ::inet_pton(AF_INET, IP.c_str(), &sockIN.sin_addr);
  sockIN.sin_port = ntohs(std::stoi(PORT));
  sockIN.sin_family = AF_INET;
  return (sockIN);
}

void WS::VirtualServer::listen()
{
  struct sockaddr_in sockIN = setSocketAddr(m_listenIP, m_listenPort);

  int opt = 1;
  if ((this->m_serverFD = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    throw (std::runtime_error("Create Socket failed\n"));
  }
  if (::setsockopt(this->m_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
  {
    throw (std::runtime_error("Socket set option failed\n"));
  }
  if (::fcntl(this->m_serverFD, F_SETFL, O_NONBLOCK) < 0)
  {
    throw (std::runtime_error("fcntl non-block failed\n"));
  }
  if (::bind(this->m_serverFD, reinterpret_cast<const sockaddr*>(&sockIN), sizeof(sockIN)) < 0)
  {
    throw (std::runtime_error("Bind Socket failed\n"));
  }
  if (::listen(this->m_serverFD, BACKLOG) < 0)
  {
    throw (std::runtime_error("Listen Socket failed\n"));
  }
  m_listenEvent = Event(EV_TYPE_ACCEPT_CONNECTION, nullptr, this, WS::handleAcceptConnection);
  m_server->attachEvent(m_serverFD, EVFILT_READ, EV_ADD | EV_CLEAR , 0, &m_listenEvent);
}

HTTP::StatusCode WS::VirtualServer::checkRequestHeader(HTTP::Request* request) const
{
  auto& headers = request->getHeaders();
  const auto requestPath = findPath(request->getPath());
  if (requestPath.empty())
    return (HTTP::ST_NOT_FOUND);
  const auto& router = m_routers.at(requestPath);

  if (!(router.isAllowedMethod(request->getMethod())))
    return (HTTP::ST_METHOD_NOT_ALLOWED);
  if (m_payloadLimit < request->getContentLength())
    return (HTTP::ST_PAYLOAD_TOO_LARGE);
  return (HTTP::ST_OK);
}

std::string WS::VirtualServer::findPath(const std::string& path) const
{
  std::string router = path;
  auto it = m_routers.begin();

  while ((it = m_routers.find(router)) == m_routers.end() && !router.empty())
  {
    router = router.substr(0, router.rfind('/'));
  }
  if (router.empty())
    return ("");
  return (it->first);
}

void WS::VirtualServer::setPayloadLimit(size_t limit)
{
  m_payloadLimit = limit;
}

void WS::VirtualServer::Get(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc)
{
  auto it = m_routers.find(path);
  if (it == m_routers.end())
  {
    m_routers.insert(std::pair<std::string, WS::Router>(path, WS::Router(path)));
    it = m_routers.find(path);
  }
  auto& router = it->second;
  router.addMethodProc("GET", proc);
}

void WS::VirtualServer::Post(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc)
{
  auto it = m_routers.find(path);
  if (it == m_routers.end())
  {
    m_routers.insert(std::pair<std::string, WS::Router>(path, WS::Router(path)));
    it = m_routers.find(path);
  }
  auto& router = it->second;
  router.addMethodProc("POST", proc);
}

void WS::VirtualServer::Put(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc)
{
  auto it = m_routers.find(path);
  if (it == m_routers.end())
  {
    m_routers.insert(std::pair<std::string, WS::Router>(path, WS::Router(path)));
    it = m_routers.find(path);
  }
  auto& router = it->second;
  router.addMethodProc("PUT", proc);
}

void WS::VirtualServer::Head(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc)
{
  auto it = m_routers.find(path);
  if (it == m_routers.end())
  {
    m_routers.insert(std::pair<std::string, WS::Router>(path, WS::Router(path)));
    it = m_routers.find(path);
  }
  auto& router = it->second;
  router.addMethodProc("HEAD", proc);
}

void WS::VirtualServer::Delete(const std::string& path, std::function<void(HTTP::Request*, HTTP::Response*)> proc)
{
  auto it = m_routers.find(path);
  if (it == m_routers.end())
  {
    m_routers.insert(std::pair<std::string, WS::Router>(path, WS::Router(path)));
    it = m_routers.find(path);
  }
  auto& router = it->second;
  router.addMethodProc("DELETE", proc);
}

void WS::VirtualServer::response(HTTP::Request* request, HTTP::Response* response)
{
  auto path = findPath(request->getPath());
  if (path.empty())
  {
    response->sendFile(m_errorPagePath.c_str());
  }
  else
  {
    auto& router = m_routers.at(path);
    auto proc = router.getMethodProc(request->getMethod());
    proc(request, response);
  }
}

void WS::VirtualServer::setErrorPage(const char* errorPagePath)
{
  m_errorPagePath = errorPagePath;
}

const char* WS::VirtualServer::getErrorPage() const
{
  return (m_errorPagePath.c_str());
}
