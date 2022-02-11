cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)

project(mypack
    VERSION 0.0.1
    DESCRIPTION "OTUS algo homework: data compression"
    LANGUAGES CXX
)

set(This mypack)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(Boost REQUIRED COMPONENTS program_options)

set (Sources
    src/main.cpp
)
add_executable(${This} ${Sources})


configure_file(src/version.h.in version.h)

target_include_directories(${This} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
)

if (MSVC)
    target_compile_options(${This} PRIVATE /W4)
else ()
    target_compile_options(${This} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()



include(CTest)
enable_testing()
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
