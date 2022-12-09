#include "Storage.hpp"

WS::Byte* WS::Storage::find(WS::Byte needle) const
{
  if (this->empty())
    return (nullptr);
  return (static_cast<WS::Byte*>(memchr(m_storage, needle, m_storedSize)));
}

WS::Byte* WS::Storage::rfind(WS::Byte needle) const
{
  if (this->empty())
    return (nullptr);
  for (size_t idx = m_storedSize - 1; ; --idx)
  {
    if (m_storage[idx] == needle)
    {
      return (&m_storage[idx]);
    }
    if (idx == 0)
      return (nullptr);
  }
}

size_t WS::Storage::findPos(WS::Byte needle) const
{
  if (this->empty())
    return (WS::Storage::NOT_FOUND);
  return (static_cast<WS::Byte*>(memchr(m_storage, needle, m_storedSize)) - this->m_storage);
}

size_t WS::Storage::rfindPos(WS::Byte needle) const
{
  if (this->empty())
    return (WS::Storage::NOT_FOUND);
  for (size_t idx = m_storedSize - 1; ; --idx)
  {
    if (m_storage[idx] == needle)
    {
      return (idx);
    }
    if (idx == 0)
      return (WS::Storage::NOT_FOUND);
  }
}

size_t WS::Storage::size() const
{
  return (m_storedSize);
}

bool WS::Storage::empty() const
{
  return (m_storedSize == 0);
}

void WS::Storage::clear()
{
  if (m_storage != nullptr)
  {
    delete (m_storage);
    m_storedSize = 0;
    m_storageSize = 0;
  }
}

void WS::Storage::append(WS::Byte* buf, size_t size)
{
  if (buf == nullptr || size == 0)
    return ;
  const size_t REMAIN_STORAGE_SIZE = m_storageSize - m_storedSize;
  const size_t NEW_STORED_SIZE = m_storedSize + size;
  const size_t NEW_STORAGE_SIZE = m_storageSize + size;

  if (REMAIN_STORAGE_SIZE < size)
  {
    WS::Byte* newStorage = new WS::Byte[NEW_STORAGE_SIZE];

    memcpy(newStorage, m_storage, m_storedSize);
    memcpy(&newStorage[m_storedSize], buf, size);
    clear();
    m_storage = newStorage;
    m_storageSize = NEW_STORAGE_SIZE;
    m_storedSize = NEW_STORED_SIZE;
  }
  else
  {
    memcpy(&m_storage[m_storedSize], buf, size);
    m_storedSize = NEW_STORED_SIZE;
  }
}

WS::Storage WS::Storage::operator+(const std::string& str)
{
  WS::Storage newStore = *this;

  newStore.append((WS::Byte*) str.c_str(), str.size());
  return (newStore);
}

WS::Storage WS::Storage::operator+(WS::CString str)
{
  if (str == nullptr)
    return (*this);
  WS::Storage newStore = *this;

  newStore.append((WS::Byte*)str, strlen(str) + 1);
  return (newStore);
}

WS::Storage WS::Storage::operator+(const WS::Storage& str)
{
  WS::Storage newStore = *this;

  newStore.append(str.m_storage, str.m_storedSize);
  return (newStore);
}

WS::Storage WS::Storage::operator+(ssize_t n)
{
  return (*this + std::to_string(n));
}

WS::Storage WS::Storage::operator+(size_t n)
{
  return (*this + std::to_string(n));
}

WS::Storage WS::Storage::operator+(double n)
{
  return (*this + std::to_string(n));
}

WS::Storage& WS::Storage::operator=(const WS::Storage& rhs)
{
  if (&rhs == this)
    return (*this);

  this->clear();
  if (rhs.empty())
  {
    this->m_storage = nullptr;
    this->m_storedSize = 0;
    this->m_storageSize = 0;
  }
  else
  {
    this->m_storage = new WS::Byte[rhs.m_storageSize];
    this->m_storageSize = rhs.m_storageSize;
    memcpy(this->m_storage, rhs.m_storage, rhs.m_storageSize);
  }
  return (*this);
}

WS::Storage& WS::Storage::operator=(const std::string& rhs)
{
  clear();
  append((Byte*) rhs.c_str(), rhs.size());
  return (*this);
}

WS::Storage& WS::Storage::operator=(WS::CString rhs)
{
  clear();
  append((Byte*) rhs, strlen(rhs) + 1);
  return (*this);
}

WS::Byte& WS::Storage::operator[](const size_t pos) const
{
  if (pos > m_storedSize)
    throw (std::runtime_error("heap buffer overflow\n"));
  return (m_storage[pos]);
}

WS::Storage::Storage():
  m_storage(nullptr),
  m_storedSize(0),
  m_storageSize(0)
{
}

WS::Storage::~Storage()
{
  clear();
}

std::ostream& operator<<(std::ostream& stream, const WS::Storage& buf)
{
  const size_t BUF_SIZE = buf.size();

  for (size_t idx = 0; idx < BUF_SIZE; ++idx)
  {
    stream << buf[idx];
  }
  return (stream);
}
