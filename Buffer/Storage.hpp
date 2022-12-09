#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include <ostream>

namespace WS
{
    typedef unsigned int Byte;
    typedef const char* CString;
    class Storage
    {
    private:
        Byte* m_storage;
        size_t m_storedSize;
        size_t m_storageSize;
    public:
        Byte* find(Byte needle) const;
        size_t findPos(Byte needle) const;
        Byte* rfind(Byte needle) const;
        size_t rfindPos(Byte needle) const;
        size_t size() const;
        void append(Byte* buf, size_t size);
        bool empty() const; // check m_storage is null
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
