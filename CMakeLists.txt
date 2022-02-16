cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)

project(rle
    VERSION 0.0.1
    DESCRIPTION "OTUS algo homework: data compression"
    LANGUAGES CXX
)

set(This rle)
set(ThisTest test_rle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(BOOST_ROOT ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})
set(BOOST_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(GTest CONFIG REQUIRED)

set (CommonSources
    src/rle.cpp
    src/rle.h
)


add_executable(${This} ${CommonSources} src/main.cpp)
configure_file(src/version.h.in version.h)

target_include_directories(${This} PRIVATE
${CMAKE_CURRENT_BINARY_DIR}
${BOOST_INCLUDE_DIR}
)

target_link_libraries(${This} ${Boost_LIBRARIES})

add_executable(${ThisTest} ${CommonSources} test/test.cpp)
target_link_libraries(${ThisTest} PRIVATE  GTest::gtest GTest::gtest_main)
target_include_directories(${ThisTest} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


if (MSVC)
    target_compile_options(${This} PRIVATE /W4)
    target_compile_options(${ThisTest} PRIVATE /W4)
else ()
    target_compile_options(${This} PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${ThisTest} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()



include(CTest)
enable_testing()

add_test(
    NAME ${ThisTest}
    COMMAND ${ThisTest}
)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
