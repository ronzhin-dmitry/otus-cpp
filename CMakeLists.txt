cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION}) 
project(fashion_mnist VERSION ${PROJECT_VERSION})

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
add_library(lib_fashion_mnist SHARED
    lib.cpp
    lib.h
)

# Установка свойств библиотеки
target_include_directories(lib_fashion_mnist
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_BINARY_DIR}
)

set_target_properties(lib_fashion_mnist PROPERTIES
    SOVERSION ${PROJECT_VERSION}
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "fashion_mnist"
)

# Исполняемый файл
add_executable(fashion_mnist main.cpp)
target_link_libraries(fashion_mnist PRIVATE lib_fashion_mnist)

find_package(Threads REQUIRED)

# Линковка с Boost и Threads
target_link_libraries(lib_fashion_mnist PRIVATE 
    Threads::Threads 
    Boost::system
)

target_link_libraries(fashion_mnist PRIVATE 
    Threads::Threads 
    Boost::system
)

# Установка компонентов
install(TARGETS lib_fashion_mnist
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
    COMPONENT lib_fashion_mnist
)

install(FILES
    lib.h
    DESTINATION include/fashion_mnist
    COMPONENT lib_fashion_mnist
)

install(TARGETS fashion_mnist 
    RUNTIME DESTINATION bin
    COMPONENT fashion_mnist
)

# Тесты
if(WITH_BOOST_TEST)
    add_executable(test_version test_version.cpp)
    target_link_libraries(test_version PRIVATE 
        Boost::unit_test_framework 
        lib_fashion_mnist 
        Threads::Threads
    )
endif()

if(MSVC)
    target_compile_options(lib_fashion_mnist PRIVATE /W4)
    target_compile_options(fashion_mnist PRIVATE /W4)
else()
    target_compile_options(lib_fashion_mnist PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(fashion_mnist PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Настройка CPack
set(CPACK_PACKAGE_CONTACT "d.v.ronjin@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fashion MNIST inference")
set(CPACK_GENERATOR "DEB")
set(CPACK_COMPONENTS_ALL lib_fashion_mnist fashion_mnist)
set(CPACK_DEB_COMPONENT_INSTALL ON)

# Настройка имен и зависимостей пакетов
set(CPACK_DEBIAN_LIB_fashion_mnist_FILE_NAME "lib_fashion_mnist_${PROJECT_VERSION}_amd64.deb")
set(CPACK_DEBIAN_fashion_mnist_FILE_NAME "fashion_mnist_${PROJECT_VERSION}_amd64.deb")
set(CPACK_DEBIAN_LIB_fashion_mnist_PACKAGE_DEPENDS "libc6, libboost-system-dev")
set(CPACK_DEBIAN_fashion_mnist_PACKAGE_DEPENDS "libc6, libboost-system-dev, lib_fashion_mnist (= ${PROJECT_VERSION})")

include(CPack)

# Включение тестов
if(WITH_BOOST_TEST)
    enable_testing()
    add_test(NAME test_version COMMAND test_version)
endif()