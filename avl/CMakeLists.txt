set(This tree)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


# Set stack size to 2 Mb
# Maximum stack is used with BSTree<> and inserting sequantial data x[i+1] > [i];
#  Stack Size = ( N/1000 * 0.65) МB (debug)
# 60 MB 
set(CMAKE_CXX_STACK_SIZE "300000000")

set (Sources
    main.cpp
)

set (Headers
    avltree.hpp
)

add_executable(${This} main.cpp)

target_include_directories(${This} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")

if (MSVC)
    target_compile_options(${This} PRIVATE /W4)
else ()
    target_compile_options(${This} PRIVATE -Wall -Wextra -pedantic -Werror)
endif() 
