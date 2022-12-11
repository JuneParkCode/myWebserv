#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include <ostream>

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
    public:
        size_t find(Byte needle) const;
        size_t find(const std::string& needle) const;
        size_t rfind(Byte needle) const;
        size_t rfind(const std::string& needle) const;
        Storage subStorage(size_t start, size_t len) const;
        void pop(size_t len); // pop storage[0] to storage[len - 1];
        void reserve(size_t size);
        size_t size() const;
        void append(Byte* buf, size_t size);
        bool empty() const;
        void clear(); // free m_storage
        Storage operator+(const std::string& str);
        Storage& operator+=(const std::string& str);
        Storage operator+(CString str);
        Storage& operator+=(CString str);
        Storage operator+(const Storage& str);
        Storage& operator+=(const Storage& str);
        Storage operator+(ssize_t n);
        Storage& operator+=(ssize_t n);
        Storage operator+(size_t n);
        Storage& operator+=(size_t n);
        Storage operator+(double n);
        Storage& operator+=(double n);
        Storage& operator=(const Storage& rhs);
        Storage& operator=(const std::string& rhs);
        Storage& operator=(CString rhs);
        Byte& operator[](size_t pos) const;
        Storage();
        ~Storage();
        // static
        static const size_t NOT_FOUND = SIZE_T_MAX;
    };
}

std::ostream& operator<<(std::ostream& stream, const WS::Storage& buf);

#endif //STORAGE_HPP
