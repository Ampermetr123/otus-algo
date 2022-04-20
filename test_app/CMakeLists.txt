set(This test_cuckoo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(Sources
   main.cpp
)

add_executable(${This} ${Sources})

target_include_directories(${This} PRIVATE
${CMAKE_SOURCE_DIR}/hashtable)

target_compile_definitions(${This} PRIVATE
   VERBOSE_LEVEL=$<$<CONFIG:Debug>:2>$<$<CONFIG:Release>:0>
)

if (MSVC)
   target_compile_options(${This} PRIVATE /W4)
else ()
   target_compile_options(${This} PRIVATE -Wall -Wextra -pedantic)
endif()


add_custom_command(TARGET ${This} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/alice.txt"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_command(TARGET ${This} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/war_and_peace.txt"
    "${CMAKE_CURRENT_BINARY_DIR}"
)
