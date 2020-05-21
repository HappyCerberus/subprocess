cmake_minimum_required(VERSION 3.10)
project(Subprocess)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -v")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

find_package(Boost 1.65.1 REQUIRED COMPONENTS iostreams system)
include_directories(${Boost_INCLUDE_DIRS})
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

add_subdirectory(abseil-cpp)

add_executable(Subprocess SubProcess.cpp ProcessInput.cpp main.cpp ProcessOutput.cpp ProcessOutput.h util/StatusOr.cpp util/StatusOr.h util/LinuxErrorHandler.cpp util/LinuxErrorHandler.h ProcessPipeInput.cpp ProcessPipeInput.h ProcessPipeOutput.cpp ProcessPipeOutput.h)
target_link_libraries(Subprocess absl::status Boost::iostreams Boost::system)