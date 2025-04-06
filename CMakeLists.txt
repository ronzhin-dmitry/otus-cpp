cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION}) 
project(mci_server VERSION ${PROJECT_VERSION})

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
add_library(lib_mci_server SHARED
    lib.cpp
    lib.h
)

# Установка свойств библиотеки
target_include_directories(lib_mci_server
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_BINARY_DIR}
)

set_target_properties(lib_mci_server PROPERTIES
    SOVERSION ${PROJECT_VERSION}
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "mci_server"
)

# Исполняемый файл
add_executable(mci_server main.cpp)
target_link_libraries(mci_server PRIVATE lib_mci_server)

find_package(Threads REQUIRED)

# Линковка с Boost и Threads
target_link_libraries(lib_mci_server PRIVATE 
    Threads::Threads 
    Boost::system
)

target_link_libraries(mci_server PRIVATE 
    Threads::Threads 
    Boost::system
)

# Установка компонентов
install(TARGETS lib_mci_server
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
    COMPONENT lib_mci_server
)

install(FILES
    lib.h
    DESTINATION include/mci_server
    COMPONENT lib_mci_server
)

install(TARGETS mci_server 
    RUNTIME DESTINATION bin
    COMPONENT mci_server
)

# Тесты
if(WITH_BOOST_TEST)
    add_executable(test_version test_version.cpp)
    target_link_libraries(test_version PRIVATE 
        Boost::unit_test_framework 
        lib_mci_server 
        Threads::Threads
    )
endif()

if(MSVC)
    target_compile_options(lib_mci_server PRIVATE /W4)
    target_compile_options(mci_server PRIVATE /W4)
else()
    target_compile_options(lib_mci_server PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(mci_server PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Настройка CPack
set(CPACK_PACKAGE_CONTACT "d.v.ronjin@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Monte Carlo Integration Server Application")
set(CPACK_GENERATOR "DEB")
set(CPACK_COMPONENTS_ALL lib_mci_server mci_server)
set(CPACK_DEB_COMPONENT_INSTALL ON)

# Настройка имен и зависимостей пакетов
set(CPACK_DEBIAN_LIB_mci_server_FILE_NAME "lib_mci_server_${PROJECT_VERSION}_amd64.deb")
set(CPACK_DEBIAN_mci_server_FILE_NAME "mci_server_${PROJECT_VERSION}_amd64.deb")
set(CPACK_DEBIAN_LIB_mci_server_PACKAGE_DEPENDS "libc6, libboost-system-dev")
set(CPACK_DEBIAN_mci_server_PACKAGE_DEPENDS "libc6, libboost-system-dev, lib_mci_server (= ${PROJECT_VERSION})")

include(CPack)

# Включение тестов
if(WITH_BOOST_TEST)
    enable_testing()
    add_test(NAME test_version COMMAND test_version)
endif()