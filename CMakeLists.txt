cmake_minimum_required(VERSION 3.16)

project(io4edge-client-c)

include(FindPackageHandleStandardArgs)

set(IO4E_PLATFORM_INCLUDE "unix")

FIND_PATH(
    IO4E_PROTOBUF_INCLUDE_DIR
      protobuf-c/protobuf-c.h
  )
FIND_LIBRARY(IO4E_PROTOBUF_LIBRARY
    NAMES libprotobuf-c.so libprotobuf-c.dylib libprotobuf-c.a protobuf-c.lib protobuf-c.dll
)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libprotobuf-c DEFAULT_MSG
    IO4E_PROTOBUF_INCLUDE_DIR
    IO4E_PROTOBUF_LIBRARY
)
IF(libprotobuf-c_FOUND)
    SET(IO4E_PROTOBUF_INCLUDE_DIRS ${IO4E_PROTOBUF_INCLUDE_DIR})
    SET(IO4E_PROTOBUF_LIBRARIES ${IO4E_PROTOBUF_LIBRARY})
    MARK_AS_ADVANCED(
    IO4E_PROTOBUF_LIBRARY
    IO4E_PROTOBUF_INCLUDE_DIR
    IO4E_PROTOBUF_DIR
    )
ELSE()
    MESSAGE(FATAL_ERROR
        "Could not find libprotobuf-c package.")
ENDIF()

find_package(Threads REQUIRED)

list(APPEND EXTRA_LIBS io4e_static)

enable_testing()


#----------------------------
# Add the project directories

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)
