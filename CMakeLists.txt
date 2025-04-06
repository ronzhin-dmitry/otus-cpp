cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION}) 
project(monte_carlo_int VERSION ${PROJECT_VERSION})

# Явно указываем стандарт C++20 для всех целей
set(CMAKE_CXX_STANDARD 20)
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
add_library(lib_monte_carlo_int SHARED
    lib.cpp
    lib.h
)

# Установка свойств библиотеки
target_include_directories(lib_monte_carlo_int
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_BINARY_DIR}
)

set_target_properties(lib_monte_carlo_int PROPERTIES
    SOVERSION ${PROJECT_VERSION}
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "monte_carlo_int"
)

# Исполняемый файл
add_executable(monte_carlo_int main.cpp)
target_link_libraries(monte_carlo_int PRIVATE lib_monte_carlo_int)

find_package(Threads REQUIRED)

# Линковка с Boost и Threads
target_link_libraries(lib_monte_carlo_int PRIVATE 
    Threads::Threads 
    Boost::system
)

target_link_libraries(monte_carlo_int PRIVATE 
    Threads::Threads 
    Boost::system
)

# Установка компонентов
install(TARGETS lib_monte_carlo_int
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
    COMPONENT lib_monte_carlo_int
)

install(FILES
    lib.h
    DESTINATION include/monte_carlo_int
    COMPONENT lib_monte_carlo_int
)

install(TARGETS monte_carlo_int 
    RUNTIME DESTINATION bin
    COMPONENT monte_carlo_int
)

# Тесты
if(WITH_BOOST_TEST)
    add_executable(test_version test_version.cpp)
    target_link_libraries(test_version PRIVATE 
        Boost::unit_test_framework 
        lib_monte_carlo_int 
        Threads::Threads
    )
endif()

if(MSVC)
    target_compile_options(lib_monte_carlo_int PRIVATE /W4)
    target_compile_options(monte_carlo_int PRIVATE /W4)
else()
    target_compile_options(lib_monte_carlo_int PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(monte_carlo_int PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Настройка CPack
set(CPACK_PACKAGE_CONTACT "d.v.ronjin@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Monte Carlo Integration Server Application")
set(CPACK_GENERATOR "DEB")
set(CPACK_COMPONENTS_ALL lib_monte_carlo_int monte_carlo_int)
set(CPACK_DEB_COMPONENT_INSTALL ON)

# Настройка имен и зависимостей пакетов
set(CPACK_DEBIAN_LIB_monte_carlo_int_FILE_NAME "lib_monte_carlo_int_${PROJECT_VERSION}_amd64.deb")
set(CPACK_DEBIAN_monte_carlo_int_FILE_NAME "monte_carlo_int_${PROJECT_VERSION}_amd64.deb")
set(CPACK_DEBIAN_LIB_monte_carlo_int_PACKAGE_DEPENDS "libc6, libboost-system-dev")
set(CPACK_DEBIAN_monte_carlo_int_PACKAGE_DEPENDS "libc6, libboost-system-dev, lib_monte_carlo_int (= ${PROJECT_VERSION})")

include(CPack)

# Включение тестов
if(WITH_BOOST_TEST)
    enable_testing()
    add_test(NAME test_version COMMAND test_version)
endif()