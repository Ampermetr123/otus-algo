cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(test_myarray)

set (Sources
    main.cpp
)

set (Headers
    locale.h
    profiler.h
    test_profile.h
    test_internal.h
    test_base.h
    test_helpers.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../myarray)

add_executable(test main.cpp)

if (MSVC)
    target_compile_options(test PRIVATE /W4)
else ()
    target_compile_options(test PRIVATE -Wall -Wextra -pedantic -Werror)
endif() 
