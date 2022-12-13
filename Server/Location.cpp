//
// Created by Sungjun Park on 2022/12/13.
//

#include "Location.hpp"

const std::vector<std::string>& WS::Location::getAllowMethods() const
{
  return m_allowMethods;
}

void WS::Location::setAllowMethods(const std::vector<std::string>& allowMethods)
{
  m_allowMethods = allowMethods;
}

const std::string& WS::Location::getRoot() const
{
  return m_root;
}

void WS::Location::setRoot(const std::string& root)
{
  m_root = root;
}

const std::vector<std::string>& WS::Location::getIndex() const
{
  return m_index;
}

void WS::Location::setIndex(const std::vector<std::string>& index)
{
  m_index = index;
}

const std::vector<std::string>& WS::Location::getAllowCGI() const
{
  return m_allowCGI;
}

void WS::Location::setAllowCGI(const std::vector<std::string>& allowCGI)
{
  m_allowCGI = allowCGI;
}

bool WS::Location::isAutoIndex() const
{
  return m_autoIndex;
}

void WS::Location::setAutoIndex(bool autoIndex)
{
  m_autoIndex = autoIndex;
}

size_t WS::Location::getClientMaxBodySize() const
{
  return m_clientMaxBodySize;
}

void WS::Location::setClientMaxBodySize(size_t mClientMaxBodySize)
{
  m_clientMaxBodySize = mClientMaxBodySize;
}

WS::Location::Location()
{
}

WS::Location::~Location()
{
}

WS::Location& WS::Location::operator=(const WS::Location& loc)
= default;
