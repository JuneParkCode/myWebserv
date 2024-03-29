#ifndef STORAGE_HPP
#define STORAGE_HPP

// STORAGE
// std::string의 경우 binary format의 데이터를 제대로 못받는 문제가 있다. (.data() 쓰면 되긴 함)
// 보다 더 효율적으로 버퍼를 사용하고자 해당 구조를 만듦
#include <string>
#include <ostream>

#define BUFFER_SIZE (10 * 1000 * 1000)

namespace WS
{
    typedef unsigned char Byte;
    typedef const char* CString;
    class Storage
    {
    private:
        Byte* m_storage;
        size_t m_storedSize;
        size_t m_storageSize;
        size_t m_cursor;
    public:
        size_t getCursor() const;
        void setCursor(size_t cursor);
        size_t find(Byte needle) const;
        size_t find(Byte needle, size_t start) const;
        size_t find(const std::string& needle) const;
        size_t find(const std::string& needle, size_t start) const;
        size_t rfind(Byte needle) const;
        size_t rfind(const std::string& needle) const;
        Storage subStorage(size_t start, size_t len) const;
        void pop(size_t len); // pop storage[0] to storage[len - 1];
        void reserve(size_t size);
        const Byte* data() const;
        size_t size() const;
        void append(const Byte* buf, size_t size);
        bool empty() const;
        void clear();
        void clearAll();
        ssize_t read(ssize_t fd);
        Storage operator+(const std::string& str);
        Storage operator+(CString str);
        Storage operator+(const Storage& str);
        Storage operator+(ssize_t n);
        Storage operator+(size_t n);
        Storage operator+(double n);
        Storage& operator+=(const std::string& str);
        Storage& operator+=(CString str);
        Storage& operator+=(const Storage& str);
        Storage& operator+=(ssize_t n);
        Storage& operator+=(size_t n);
        Storage& operator+=(double n);
        Storage& operator=(const Storage& rhs);
        Storage& operator=(const std::string& rhs);
        Storage& operator=(CString rhs);
        Byte& operator[](size_t pos) const;
        Storage();
        Storage(size_t storageSize);
        Storage(const Storage& storage);
        Storage(Storage&& storage) noexcept ;
        ~Storage();
        // static
        static const size_t NOT_FOUND = SIZE_T_MAX;
    };
}

std::ostream& operator<<(std::ostream& stream, const WS::Storage& buf);

#endif //STORAGE_HPP
