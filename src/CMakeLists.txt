# SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
# SPDX-License-Identifier: Apache-2.0

file(GLOB SOURCES "*.c")
file(GLOB_RECURSE API_SOURCES "api/*.c")
file(GLOB PS_SOURCES "${IO4E_PLATFORM_INCLUDE}/*.c")

# static library
add_library(io4edge STATIC ${SOURCES} ${PS_SOURCES} ${API_SOURCES})

# copy api headers to include folder
set(API_INCLUDE_COPY_PATH "${CMAKE_CURRENT_LIST_DIR}/include/io4edge_api")
set(API_SRC "${CMAKE_CURRENT_LIST_DIR}/api")

file(COPY "${API_SRC}/io4edge/protobuf-c/core_api/v1alpha2/io4edge_core_api.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/io4edge/protobuf-c/functionblock/v1alpha1/io4edge_functionblock.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/io4edge/protobuf-c/functionblock/v1alpha1/google/protobuf/any.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}/google/protobuf")
file(COPY "${API_SRC}/analogInTypeA/protobuf-c/analogInTypeA/v1alpha1/analogInTypeA.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/binaryIoTypeA/protobuf-c/binaryIoTypeA/v1alpha1/binaryIoTypeA.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/binaryIoTypeB/protobuf-c/binaryIoTypeB/v1alpha1/binaryIoTypeB.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/binaryIoTypeC/protobuf-c/binaryIoTypeC/v1alpha1/binaryIoTypeC.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/canL2/protobuf-c/canL2/v1alpha1/canL2.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/motionSensor/protobuf-c/motionSensor/v1/motionSensor.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/mvbSniffer/protobuf-c/mvbSniffer/v1/mvbSniffer.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/mvbSniffer/protobuf-c/mvbSniffer/v1/telegram.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")
file(COPY "${API_SRC}/templateInterface/protobuf-c/templateInterface/v1alpha1/templateInterface.pb-c.h" DESTINATION "${API_INCLUDE_COPY_PATH}")


target_include_directories(io4edge
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/io4edge_api ${CMAKE_CURRENT_SOURCE_DIR} ${IO4E_PLATFORM_INCLUDE})

target_link_libraries(io4edge ${IO4E_PROTOBUF_LIBRARY})
install(TARGETS io4edge ARCHIVE DESTINATION ${IO4E_LIBDIR})
install(DIRECTORY include/ DESTINATION ${IO4E_INCLUDE_DIR})
