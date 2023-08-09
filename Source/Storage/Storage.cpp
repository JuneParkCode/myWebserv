#include "Storage.hpp"
#include <unistd.h>

// storage move constructor
WS::Storage::Storage(WS::Storage&& storage) noexcept:
        m_storage(storage.m_storage),
        m_storageSize(storage.m_storageSize),
        m_storedSize(storage.m_storedSize),
        m_cursor(0)
{
  storage.m_storage = nullptr;
  storage.m_storedSize = 0;
  storage.m_storageSize = 0;
  storage.m_cursor = 0;
}

WS::Storage::Storage(const WS::Storage& storage)
{
  *this = storage;
}

WS::Storage::Storage():
        m_storage(nullptr),
        m_storedSize(0),
        m_storageSize(0),
        m_cursor(0)
{
}

WS::Storage::Storage(size_t storageSize):
        m_storage(new Byte[storageSize]),
        m_storedSize(0),
        m_storageSize(storageSize),
        m_cursor(0)
{
}

WS::Storage::~Storage()
{
  delete[] (m_storage);
}

size_t WS::Storage::find(WS::Byte needle) const
{
  if (this->empty())
    return (WS::Storage::NOT_FOUND);
  return (static_cast<WS::Byte*>(memchr(m_storage, needle, m_storedSize)) - this->m_storage);
}

size_t WS::Storage::find(WS::Byte needle, size_t start) const
{
  if (this->empty())
    return (WS::Storage::NOT_FOUND);
  return (static_cast<WS::Byte*>(memchr(&m_storage[start], needle, m_storedSize)) - this->m_storage);
}

static bool isMatched(const WS::Byte* bytes, const std::string& needle, const size_t byteSize)
{
  if (needle.size() > byteSize)
    return (false);
  return (memcmp(bytes, needle.c_str(), needle.length()) == 0);
}

size_t WS::Storage::find(const std::string& needle) const
{
  if (this->empty())
    return (WS::Storage::NOT_FOUND);
  for (size_t idx = 0; idx < m_storedSize; ++idx)
  {
    if (isMatched(&m_storage[idx], needle, (m_storedSize - idx)))
      return (idx);
  }
  return (WS::Storage::NOT_FOUND);
}

size_t WS::Storage::find(const std::string& needle, size_t start) const
{
  if (this->empty())
    return (WS::Storage::NOT_FOUND);
  for (size_t idx = start; idx < m_storedSize; ++idx)
  {
    if (isMatched(&m_storage[idx], needle, (m_storedSize - idx)))
      return (idx);
  }
  return (WS::Storage::NOT_FOUND);
}

size_t WS::Storage::rfind(const std::string& needle) const
{
  if (this->empty())
    return (WS::Storage::NOT_FOUND);
  for (size_t idx = m_storedSize - 1; ; --idx)
  {
    if (isMatched(&m_storage[idx], needle, (m_storedSize - idx)))
      return (idx);
    if (idx == 0)
      return (WS::Storage::NOT_FOUND);
  }
}

size_t WS::Storage::rfind(WS::Byte needle) const
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
  m_storedSize = 0;
  m_cursor = 0;
}

void WS::Storage::clearAll()
{
  delete[] (m_storage);
  m_storage = nullptr;
  m_storedSize = 0;
  m_storageSize = 0;
  m_cursor = 0;
}

void WS::Storage::append(const WS::Byte* buf, size_t size)
{
  if (buf == nullptr || size == 0)
    return ;
  const size_t NEW_STORED_SIZE = m_storedSize + size;

  if (NEW_STORED_SIZE > m_storageSize)
    reserve(m_storageSize * 2);
  ::memcpy(&m_storage[m_storedSize], buf, size);
  m_storedSize = NEW_STORED_SIZE;
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

WS::Storage& WS::Storage::operator+=(const std::string& str)
{
  *this = *this + str;
  return (*this);
}

WS::Storage& WS::Storage::operator+=(WS::CString str)
{
  *this = *this + str;
  return (*this);
}

WS::Storage& WS::Storage::operator+=(const WS::Storage& str)
{
  *this = *this + str;
  return (*this);
}

WS::Storage& WS::Storage::operator+=(ssize_t n)
{
  *this = *this + n;
  return (*this);
}

WS::Storage& WS::Storage::operator+=(size_t n)
{
  *this = *this + n;
  return (*this);
}

WS::Storage& WS::Storage::operator+=(double n)
{
  *this = *this + n;
  return (*this);
}

WS::Storage& WS::Storage::operator=(const WS::Storage& rhs)
{
  if (&rhs == this)
    return (*this);

  reserve(rhs.m_storageSize);
  if (!rhs.empty())
    ::memcpy(this->m_storage, rhs.m_storage, rhs.m_storedSize);
  this->m_storedSize = rhs.m_storedSize;
  this->m_cursor = rhs.m_cursor;
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
    throw (std::runtime_error("Storage overflow\n"));
  return (m_storage[pos]);
}

WS::Storage WS::Storage::subStorage(size_t start, size_t len) const
{
  if (len == 0)
    return (WS::Storage());
  if (start == 0 && len >= m_storedSize)
  {
    WS::Storage newStorage;
    newStorage = *this;
    return (newStorage);
  }
  else
  {
    WS::Storage newStorage;
    newStorage.reserve(len + 1);
    ::memcpy(newStorage.m_storage, &m_storage[start], len);
    newStorage.m_storedSize = len;
    newStorage.append((WS::Byte*)"\0", 1);
    return (newStorage);
  }
}

void WS::Storage::pop(size_t len)
{
  if (m_storedSize == 0 || m_storageSize == 0 || len == 0)
    return ;
  if (m_storedSize <= len)
  {
    m_storedSize = 0;
    m_cursor = 0;
  }
  else
  {
    ::memmove(m_storage, &m_storage[len], (m_storageSize - len));
    m_storedSize = m_storageSize - len;
    m_cursor -= len;
  }
}

void WS::Storage::reserve(size_t size)
{
  if (m_storageSize >= size || size == 0)
    return ;
  else
  {
    auto newStorage = new WS::Byte[size];
    size_t temp = m_storedSize;

    ::memcpy(newStorage, m_storage, m_storedSize);
    delete[] (m_storage);
    m_storage = newStorage;
    m_storedSize = temp;
    m_storageSize = size;
  }
}

const WS::Byte* WS::Storage::data() const
{
  return (m_storage);
}

size_t WS::Storage::getCursor() const
{
  return (m_cursor);
}

void WS::Storage::setCursor(size_t cursor)
{
  m_cursor = cursor;
}

ssize_t WS::Storage::read(ssize_t fd)
{
  if (m_storageSize < BUFFER_SIZE + m_storedSize)
    reserve(m_storageSize * 2);
  ssize_t readSize = ::read(fd, &m_storage[m_storedSize], BUFFER_SIZE);

  if (readSize > 0)
  {
    m_storedSize += readSize;
  }
  return (readSize);
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
