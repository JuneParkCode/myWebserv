cmake_minimum_required(VERSION 3.13)

project("miniWebserv")

set(CMAKE_CXX_STANDARD 11)

#add_compile_options(-g3 -fsanitize=address)
#add_link_options(-g3 -fsanitize=address)

set(SOURCE_FILES
        Storage/Storage.cpp
        HTTP/Request.cpp
        HTTP/Response.cpp
        Server/Connection.cpp
        Server/Handlers.cpp
        Server/Location.cpp
        Server/Server.cpp
        Server/VirtualServer.cpp
        ThreadPool/ThreadPool.cpp
        )

add_executable(miniWebserv
        ${SOURCE_FILES}
        )

target_include_directories(miniWebserv
        PUBLIC
        Event
        HTTP
        Storage
        Server
        ThreadPool
        )
