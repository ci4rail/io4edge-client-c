file(GLOB EXAMPLES_SOURCES RELATIVE ${PROJECT_SOURCE_DIR}/examples *.c)


foreach(examples_src ${EXAMPLES_SOURCES})

    # Remove the suffix so that it becomes the executable name
    string(REPLACE ".c" "" examplename ${examples_src})
    set(exampleexe "io4e-${examplename}")
    add_executable(${exampleexe} ${PROJECT_SOURCE_DIR}/examples/${examples_src})
    target_link_libraries(${exampleexe} io4e_static ${EXTRA_LIBS})

endforeach()