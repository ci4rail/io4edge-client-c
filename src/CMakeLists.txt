file(GLOB SOURCES "*.c")
file(GLOB_RECURSE API_SOURCES "api/*.c")
file(GLOB PS_SOURCES "${IO4E_PLATFORM_INCLUDE}/*.c")

# static library
add_library(io4e_static STATIC ${SOURCES} ${PS_SOURCES} ${API_SOURCES})

# workaround for any.pb-c.c
set(EXTRA_INCLUDE "api/io4edge/protobuf-c/functionblock/v1alpha1")

target_include_directories(io4e_static
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/api
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/api ${CMAKE_CURRENT_SOURCE_DIR} ${IO4E_PLATFORM_INCLUDE} ${EXTRA_INCLUDE})

target_link_libraries(io4e_static ${IO4E_PROTOBUF_LIBRARY})
