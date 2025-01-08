cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(editor VERSION ${PROJECT_VESRION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

configure_file(version.h.in version.h)

add_executable(matrix main.cpp)

target_include_directories(matrix
    PRIVATE "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}"
)

add_library(matrix_lib lib.cpp lib.h)
target_include_directories(matrix_lib
    PRIVATE "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(matrix PUBLIC
    matrix_lib
)

set_target_properties(matrix matrix_lib PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    add_executable(test_version test_version.cpp)

    set_target_properties(test_version PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    set_target_properties(test_version PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
    )

    target_link_libraries(test_version
        ${Boost_LIBRARIES}
        matrix_lib
    )
endif()

if (MSVC)
    target_compile_options(matrix PRIVATE
        /W4
    )
    target_compile_options(matrix_lib PRIVATE
        /W4
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(matrix PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(matrix_lib PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

install(TARGETS matrix RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()