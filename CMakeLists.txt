cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(async VERSION ${PROJECT_VESRION})
# Явно указываем стандарт C++17 для всех целей
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

configure_file(version.h.in version.h)

# Создание shared-библиотеки
add_library(async SHARED
    lib.cpp
    lib.h
    async.cpp
    async.h
)

# Установка свойств библиотеки
target_include_directories(async
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_BINARY_DIR}
)

set_target_properties(async PROPERTIES
    SOVERSION ${PROJECT_VERSION}
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "async"
)

# Исполняемый файл
add_executable(bulk main.cpp)
target_link_libraries(bulk PRIVATE async)

# Установка
install(TARGETS async
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(FILES
    lib.h
    async.h
    DESTINATION include/async
)

install(TARGETS bulk RUNTIME DESTINATION bin)

# Тесты
if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    add_executable(test_version test_version.cpp)
    target_link_libraries(test_version PRIVATE Boost::unit_test_framework async)
endif()

if(MSVC)
    target_compile_options(async PRIVATE /W4)
    target_compile_options(bulk PRIVATE /W4)
else()
    target_compile_options(async PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Настройка CPack
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "async")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
include(CPack)

# Включение тестов
if(WITH_BOOST_TEST)
    enable_testing()
    add_test(NAME test_version COMMAND test_version)
endif()