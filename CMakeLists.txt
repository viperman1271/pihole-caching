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

add_executable(pihole-caching main.cpp queryAnswerType.h queryDnsType.h objectmodel/queryinfo.h objectmodel/queryresult.h)
target_link_libraries(pihole-caching dnsapi)