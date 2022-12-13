//
// Created by Sungjun Park on 2022/12/13.
//

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <map>
#include <vector>

namespace WS
{
  class Location
  {
  private:
      std::string m_path;
      std::vector<std::string> m_allowMethods;
      std::string m_root;
      std::vector<std::string> m_index;
      std::vector<std::string> m_allowCGI;
      bool m_autoIndex{};
      size_t m_clientMaxBodySize{};
  public:
      const std::vector<std::string>& getAllowMethods() const;
      void setAllowMethods(const std::vector<std::string>& allowMethods);
      const std::string& getRoot() const;
      void setRoot(const std::string& root);
      const std::vector<std::string>& getIndex() const;
      void setIndex(const std::vector<std::string>& index);
      const std::vector<std::string>& getAllowCGI() const;
      void setAllowCGI(const std::vector<std::string>& allowCGI);
      bool isAutoIndex() const;
      void setAutoIndex(bool autoIndex);
      size_t getClientMaxBodySize() const;
      void setClientMaxBodySize(size_t clientMaxBodySize);
      std::string getFilePath(const std::string& url) const;
      bool isValidLocationURL(const std::string& url) const;
  public:
      Location();
      ~Location();
      Location& operator=(const Location& loc);
  };
}

#endif //LOCATION_HPP
