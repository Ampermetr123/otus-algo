cmake_minimum_required(VERSION 3.1)

set(PYTHON_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../venv/lib/site-packages)
set(PYTHON_EXECUTABLE  ${CMAKE_CURRENT_SOURCE_DIR}/../venv/Scripts/python.exe)
set(pybind11_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../venv/lib/site-packages/pybind11/share/cmake/pybind11/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# set(CMAKE_CXX_FLAGS "-O3")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(myarray)

file (GLOB SOURCE_FILES "../array/*.cpp")
file (GLOB HEADER_FILES "../array/*.hpp")
file (GLOB PYTHON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")


find_package(pybind11 REQUIRED)
pybind11_add_module(myarray 
${SOURCE_FILES}
${HEADER_FILES}
${PYTHON_FILES}
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../array"
${pybind11_INCLUDE_DIRS}
)

target_link_libraries(myarray PUBLIC)

install(TARGETS myarray
COMPONENT python
LIBRARY DESTINATION "${PYTHON_LIBRARY_DIR}"
)