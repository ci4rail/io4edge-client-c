file(GLOB SOURCES "*.c")
file(GLOB_RECURSE API_SOURCES "api/*.c")
file(GLOB PS_SOURCES "${IO4E_PLATFORM_INCLUDE}/*.c")

# static library
add_library(io4edge STATIC ${SOURCES} ${PS_SOURCES} ${API_SOURCES})

set(API_INCLUDE_COPY_PATH "${CMAKE_CURRENT_LIST_DIR}/include/io4edge_api")
set(API_SRC "${CMAKE_CURRENT_LIST_DIR}/api")

# copy api headers to include folder
file(COPY "${API_SRC}/io4edge/protobuf-c/core_api/v1alpha2/io4edge_core_api.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/io4edge/protobuf-c/functionblock/v1alpha1/io4edge_functionblock.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/binaryIoTypeA/protobuf-c/binaryIoTypeA/v1alpha1/binaryIoTypeA.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/io4edge/protobuf-c/functionblock/v1alpha1/google/protobuf/any.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}/google/protobuf")


target_include_directories(io4edge
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/io4edge_api ${CMAKE_CURRENT_SOURCE_DIR} ${IO4E_PLATFORM_INCLUDE})

target_link_libraries(io4edge ${IO4E_PROTOBUF_LIBRARY})
install(TARGETS io4edge ARCHIVE DESTINATION ${IO4E_LIBDIR})
message(STATUS "IO4E_INCLUDE_DIR: ${IO4E_INCLUDE_DIR} ${CMAKE_INSTALL_INCLUDEDIR}")
install(DIRECTORY include/ DESTINATION ${IO4E_INCLUDE_DIR})
