cmake_minimum_required(VERSION 3.17)
project(server)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
include_directories(includes)

add_executable(server_cpp
        src/main.cpp
        src/Cgi.cpp
        src/Cluster.cpp
        includes/Cluster.hpp
        src/ServerConfig.cpp
        includes/ServerConfig.hpp
        src/Server.cpp
        includes/Server.hpp
        src/ParseConfig.cpp
        includes/ParseConfig.hpp
        src/Connection.cpp
        includes/Connection.hpp
        includes/Exceptions.hpp
        src/Response.cpp
        includes/Response.hpp
        src/RequestHandler.cpp
        includes/RequestHandler.hpp
        src/get_next_line/get_next_line.h
        src/get_next_line/get_next_line_utils.cpp
        src/get_next_line/get_next_line.cpp
        includes/Defenitions.hpp
        includes/utils.hpp
        src/utils.cpp
        includes/Cgi.hpp
        src/Cgi.cpp)
add_executable(server_old_c
                server.cpp)

add_executable(server_test
        src/Cluster.cpp
                includes/Cluster.hpp
                src/ServerConfig.cpp
                includes/ServerConfig.hpp
                src/Server.cpp
                includes/Server.hpp
        )