cmake_minimum_required(VERSION 3.10)

project(CTestingExample
    VERSION 1.0.4
    DESCRIPTION "Simple C library with unit testing and code coverage"
    LANGUAGES C
)

# Set C standard
if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
endif()
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler warnings (MSVC vs GCC/Clang)
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Option to enable code coverage (default: ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" ON)

# Coverage configuration (only for GCC/Clang)
if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Code coverage enabled")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(-fprofile-arcs -ftest-coverage)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    else()
        message(WARNING "Code coverage requested but compiler ${CMAKE_C_COMPILER_ID} is not supported")
        set(ENABLE_COVERAGE OFF)
    endif()
endif()

# Library
add_library(testlib STATIC
    lib.c
    lib.h
)
target_include_directories(testlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Extra libs: only on non-Windows (Windows usually has no ncurses, and no libm)
set(EXTRA_LIBS "")
if(NOT WIN32)
    list(APPEND EXTRA_LIBS m ncurses)
endif()

# Unit test executable
add_executable(test-library
    test-library.c
)
target_link_libraries(test-library
    testlib
    ${EXTRA_LIBS}
)

# Integration test executable
add_executable(integration-test
    integration-test.c
)
target_link_libraries(integration-test
    testlib
    ${EXTRA_LIBS}
)

# Enable testing
enable_testing()

# Register tests with CTest (use TARGET_FILE so CTest finds correct exe path)
add_test(NAME LibraryTests COMMAND $<TARGET_FILE:test-library>)
add_test(NAME IntegrationTests COMMAND $<TARGET_FILE:integration-test>)

# Coverage target (mainly for Linux CI)
if(ENABLE_COVERAGE)
    find_program(GCOV_EXECUTABLE gcov)
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(GCOV_EXECUTABLE)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E remove_directory coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${GCOV_EXECUTABLE} -b -c lib.c
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated in *.gcov files"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS test-library integration-test
            COMMENT "Generating code coverage report..."
        )

        if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
            add_custom_command(TARGET coverage POST_BUILD
                COMMAND ${LCOV_EXECUTABLE} --capture --directory . --output-file coverage.info --ignore-errors unused,unsupported
                COMMAND ${LCOV_EXECUTABLE} --remove coverage.info '*/test-library.c' '*/integration-test.c'
                        --output-file coverage.info --ignore-errors unused,unsupported
                COMMAND ${GENHTML_EXECUTABLE} coverage.info --output-directory coverage --ignore-errors unused,unsupported
                COMMAND ${CMAKE_COMMAND} -E echo "HTML coverage report generated in coverage/index.html"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
        endif()
    else()
        message(STATUS "gcov not found - coverage target will not be available")
    endif()
endif()

message(STATUS "")
message(STATUS "Configuration Summary:")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Coverage Enabled: ${ENABLE_COVERAGE}")
message(STATUS "")
