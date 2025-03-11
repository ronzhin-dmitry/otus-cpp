cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION}) 
project(async VERSION ${PROJECT_VERSION})

# Явно указываем стандарт C++17 для всех целей
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

# Поиск необходимых компонентов Boost
if(WITH_BOOST_TEST)
    set(BOOST_COMPONENTS system unit_test_framework)
else()
    set(BOOST_COMPONENTS system)
endif()
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

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

find_package(Threads REQUIRED)

# Линковка с Boost и Threads
target_link_libraries(async PRIVATE 
    Threads::Threads 
    Boost::system
)

target_link_libraries(bulk PRIVATE 
    Threads::Threads 
    Boost::system
)

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
    add_executable(test_version test_version.cpp)
    target_link_libraries(test_version PRIVATE 
        Boost::unit_test_framework 
        async 
        Threads::Threads
    )
endif()

if(MSVC)
    target_compile_options(async PRIVATE /W4)
    target_compile_options(bulk PRIVATE /W4)
else()
    target_compile_options(async PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Настройка CPack
set(CPACK_PACKAGE_CONTACT "d.v.ronjin@gmail.com")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libpthread-stubs0-dev")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Async project for handling bulk operations")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "async")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
include(CPack)

# Включение тестов
if(WITH_BOOST_TEST)
    enable_testing()
    add_test(NAME test_version COMMAND test_version)
endif()