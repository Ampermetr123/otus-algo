set(This hashtbale)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

enable_testing()

set(Sources 
test_hashtable.cpp
)

set(Headers 
hashtable.hpp
)

add_executable(${This} ${Sources})

target_link_libraries(
   ${This}
   gtest_main
   gmock
)

include(GoogleTest)
gtest_discover_tests(${This})