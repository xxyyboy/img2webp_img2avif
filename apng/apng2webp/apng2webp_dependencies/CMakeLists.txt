cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(apng2webp_dependecies)
set(apng2webp_dependecies_VERSION_MAJOR 1)
set(apng2webp_dependecies_VERSION_MINOR 0)

add_compile_options(-std=c++11)

add_executable(apng2webp_apngopt apng2webp_apngopt/apngopt.cpp)
add_executable(apngdisraw apngdisraw/apngdis.cpp)

if(STATIC_LINKING)
# Windows MinGW use actually statically linked binary
    if (MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
        set_target_properties(apng2webp_apngopt PROPERTIES LINK_SEARCH_START_STATIC ON)
        set_target_properties(apng2webp_apngopt PROPERTIES LINK_SEARCH_END_STATIC ON)
        set_target_properties(apngdisraw PROPERTIES LINK_SEARCH_START_STATIC ON)
        set_target_properties(apngdisraw PROPERTIES LINK_SEARCH_END_STATIC ON)
    endif(MINGW)
    set(BUILD_SHARED_LIBRARIES OFF)
endif(STATIC_LINKING)

if(STATIC_LINKING)
# OS X provide system zlib .dylib but not static .a
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        find_package(ZLIB REQUIRED)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        find_package(ZLIB REQUIRED)
    endif()
else(STATIC_LINKING)
    find_package(ZLIB REQUIRED)
endif(STATIC_LINKING)

set(PNG_INCLUDE_DIRS /data/nginx/SecNginx1202/3partylib/libpng1639/include/)
set(PNG_LIBRARIES /data/nginx/SecNginx1202/3partylib/libpng1639/lib/libpng.a)
set(PNG_PNG_INCLUDE_DIR /data/nginx/SecNginx1202/3partylib/libpng1639/include/)
set(PNG_LIBRARY /data/nginx/SecNginx1202/3partylib/libpng1639/lib/libpng.a)

find_package(PNG REQUIRED)

set(Jsoncpp_INCLUDE_DIR /usr/local/include/json/)
set(Jsoncpp_LIBRARY /data/nginx/SecNginx1202/3partylib/jsoncpp-1.9.5/build/lib/libjsoncpp.a)
#set(Jsoncpp_LIBRARY /usr/local/lib64/)
find_package(Jsoncpp REQUIRED)

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${Jsoncpp_INCLUDE_DIRS})

target_link_libraries(apng2webp_apngopt ${ZLIB_LIBRARIES})
target_link_libraries(apng2webp_apngopt ${PNG_LIBRARIES})
target_link_libraries(apngdisraw ${PNG_LIBRARIES})
target_link_libraries(apngdisraw ${Jsoncpp_LIBRARIES})

install(TARGETS apng2webp_apngopt apngdisraw DESTINATION bin)
