cmake_minimum_required(VERSION 3.17)
project(server)

set(CMAKE_CXX_STANDARD 98)

include_directories(src)
include_directories(includes)

add_executable(server_cpp src/main.cpp src/Cluster.cpp includes/Cluster.hpp src/ServerConfig.cpp includes/ServerConfig.hpp src/Server.cpp includes/Server.hpp src/Parser.cpp includes/Parser.hpp src/Connection.cpp includes/Connection.hpp includes/Errors.hpp)
add_executable(server_old_c server.cpp)
add_executable(server_test src/test.cpp src/Cluster.cpp includes/Cluster.hpp src/ServerConfig.cpp includes/ServerConfig.hpp src/Server.cpp includes/Server.hpp src/Parser.cpp includes/Parser.hpp)