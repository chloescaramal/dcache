cmake_minimum_required(VERSION 4.1)
project(dcache)

set(CMAKE_CXX_STANDARD 14)

add_executable(dCacheClient
        ../dCacheClient.c
        ../communication/message.c
        ../storage/datastorage.c)

add_executable(dCacheServer
        ../dCacheServer.c
        ../communication/message.c
        ../storage/datastorage.c
        ../server-service/dcacheserver.h
        ../server-service/dcacheserver.c)