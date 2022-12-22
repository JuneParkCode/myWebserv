cmake_minimum_required(VERSION 3.13)

project("miniWebserv")

set(CMAKE_CXX_STANDARD 11)

#add_compile_options(-g3 -fsanitize=address)
#add_link_options(-g3 -fsanitize=address)

set(SOURCE_FILES
        Source/Event/Event.cpp
        Source/Server/Connection.cpp
        Source/Server/Handlers.cpp
        Source/Server/Router.cpp
        Source/Server/Server.cpp
        Source/Server/VirtualServer.cpp
        Source/ThreadPool/ThreadPool.cpp
        Source/Storage/Storage.cpp
        webserv.cpp
        Source/HTTP/Request.cpp
        Source/HTTP/Response.cpp
        Source/HTTP/RequestParser.cpp
        Source/HTTP/RequestProcessor.cpp
        Source/HTTP/HTTPDefinitions.cpp)

add_executable(miniWebserv
        ${SOURCE_FILES}
        )

target_include_directories(miniWebserv
        PUBLIC
        Source/Event
        Source/Server
        Source/ThreadPool
        Source/ARequest
        Source/AResponse
        Source/Storage
        Source/HTTP
        )
