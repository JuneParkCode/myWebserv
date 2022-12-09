cmake_minimum_required(VERSION 3.13)

project("miniWebserv")

set(CMAKE_CXX_STANDARD 11)

#add_compile_options(-g3 -fsanitize=address)
#add_link_options(-g3 -fsanitize=address)

set(SOURCE_FILES
        Buffer/Storage.cpp
        HTTP/Request.cpp)

add_executable(miniWebserv
        ${SOURCE_FILES}
        )

target_include_directories(miniWebserv
        PUBLIC
        Buffer
        HTTP
        )
