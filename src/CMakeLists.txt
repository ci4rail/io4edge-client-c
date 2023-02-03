file(GLOB SOURCES "*.c")
file(GLOB PS_SOURCES "${IO4E_PLATFORM_INCLUDE}/*.c")

# static library
add_library(io4e_static STATIC ${SOURCES} ${PS_SOURCES})

target_include_directories(io4e_static
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR} ${IO4E_PLATFORM_INCLUDE})
