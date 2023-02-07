include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/fd36851c8d383a2b711905eb488692da1152a3d8.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


include(GoogleTest)

file(GLOB TEST_SOURCES RELATIVE ${PROJECT_SOURCE_DIR}/tests *.cc)

foreach(test_src ${TEST_SOURCES})

    # Remove the suffix so that it becomes the executable name
    string(REPLACE ".cc" "" testname ${test_src})
    set(testexe "${testname}")
    add_executable(${testexe} ${PROJECT_SOURCE_DIR}/tests/${test_src})
    target_include_directories(${testexe} PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${testexe} io4e_static ${EXTRA_LIBS} GTest::gtest_main)
    gtest_discover_tests(${testexe})
endforeach()
