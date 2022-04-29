set(This utest)


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
   # Linux-specific stuff
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.11.0.zip
  )
  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  endif()
  add_library(GTest::GTest INTERFACE IMPORTED)
  target_link_libraries(GTest::GTest INTERFACE gtest_main)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # use vcpkg on windows
   find_package(gtest REQUIRED)
   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

enable_testing()

set(Sources
   test_cuckoo.cpp
   test_location.cpp
)

set(Headers
   ${CMAKE_SOURCE_DIR}/hashtable/location.h
   ${CMAKE_SOURCE_DIR}/hashtable/cuckoo_hashtable.h
   ${CMAKE_SOURCE_DIR}/hashtable/cuckoo_traits.h
)

add_executable(${This} ${Sources})

target_link_libraries(
   ${This}
   GTest::GTest
   GTest::Main
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
