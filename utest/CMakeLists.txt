set(This utest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


enable_testing()

set(Sources
#test_hashtable.cpp
test_cuckoo.cpp
test_location.cpp
)

set(Headers
${CMAKE_SOURCE_DIR}/hashtable/location.h
${CMAKE_SOURCE_DIR}/hashtable/cuckoo_hashtable.h
)

add_executable(${This} ${Sources})

target_link_libraries(
   ${This}
   gtest_main
   gmock
)

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


include(GoogleTest)
gtest_discover_tests(${This})
