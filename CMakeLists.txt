cmake_minimum_required (VERSION 2.6)
project (pihole-caching)

set (pihole-caching_VERSION_MAJOR 1)
set (pihole-caching_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/pihole-caching.hxx"
  "${PROJECT_BINARY_DIR}/pihole-caching.generated.h"
  )
  
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/pihole-caching.hxx"
  "${PROJECT_BINARY_DIR}/pihole-caching.generated.h"
  )

add_executable(pihole-caching main.cpp queryAnswerType.h queryDnsType.h objectmodel/queryInfo.h objectmodel/queryResult.h)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

if(WIN32)
    target_link_libraries(pihole-caching dnsapi)
endif()

if(UNIX)
    include(~/vcpkg/scripts/buildsystems/vcpkg.cmake)
    find_package(json-c CONFIG REQUIRED)
    find_package(CURL REQUIRED)
    find_package(OpenSSL REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(Threads)
    target_link_libraries(pihole-caching PRIVATE json-c::json-c ${CURL_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB ${CMAKE_THREAD_LIBS_INIT} resolv)
    target_include_directories(pihole-caching PRIVATE ${CURL_INCLUDE_DIRS})
endif()
