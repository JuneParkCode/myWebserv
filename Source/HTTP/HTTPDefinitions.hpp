#ifndef HTTP_DEFINITIONS_HPP
#define HTTP_DEFINITIONS_HPP

#include <string>

namespace HTTP
{
    enum StatusCode
    {
        ST_CONTINUE = 100,
        ST_SWITCHING_PROTOCOLS = 101,
        ST_OK = 200,
        ST_CREATED = 201,
        ST_ACCEPTED = 202,
        ST_NON_AUTHORITATIVE_INFORMATION = 203,
        ST_NO_CONTENT = 204,
        ST_RESET_CONTENT = 205,
        ST_PARTIAL_CONTENT = 206,
        ST_MULTIPLE_CHOICES = 300,
        ST_MOVED_PERMANENTLY = 301,
        ST_FOUND = 302,
        ST_SEE_OTHER = 303,
        ST_NOT_MODIFIED = 304,
        ST_TEMPORARY_REDIRECT = 307,
        ST_PERMANENT_REDIRECT = 308,
        ST_BAD_REQUEST = 400,
        ST_UNAUTHORIZED = 401,
        ST_FORBIDDEN = 403,
        ST_NOT_FOUND = 404,
        ST_METHOD_NOT_ALLOWED = 405,
        ST_REQUEST_TIMEOUT = 408,
        ST_LENGTH_REQUIRED = 411,
        ST_PAYLOAD_TOO_LARGE = 413,
        ST_INTERNAL_SERVER_ERROR = 500,
        ST_NOT_IMPLEMENTED = 501,
        ST_BAD_GATEWAY = 502,
        ST_SERVICE_UNAVAILABLE = 503,
        ST_GATEWAY_TIMEOUT = 504,
        ST_HTTP_VERSION_NOT_SUPPORTED = 505,
        ST_ERROR = -1
    };

    enum AvailableMethods
    {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        UNKNOWN
    };
    static const char* AVAILABLE_METHODS[] = {"GET", "HEAD", "POST", "PUT", "DELETE"};
    AvailableMethods methodStringToEnum(const std::string& method);
    std::string methodTypeToString(AvailableMethods method);
}

// FIXME : 이후에 다른 서버 정의 쪽으로 빼는 것이 적절할 것 같다.
typedef int FileDescriptor;


#endif //HTTP_DEFINITIONS_HPP
