file(GLOB_RECURSE EXAMPLES_SOURCES RELATIVE ${PROJECT_SOURCE_DIR}/examples *.c)

foreach(examples_src ${EXAMPLES_SOURCES})
    get_filename_component(exampledir ${examples_src} DIRECTORY)
    get_filename_component(examplefile ${examples_src} NAME)

    # Remove the suffix so that it becomes the executable name
    string(REPLACE ".c" "" examplename ${examplefile})
    set(exampleexe "io4e-${exampledir}-${examplename}")
    add_executable(${exampleexe} ${PROJECT_SOURCE_DIR}/examples/${examples_src})

    target_link_libraries(${exampleexe} io4edge ${EXTRA_LIBS} Threads::Threads)

endforeach()
