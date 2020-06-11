cmake_minimum_required(VERSION 3.15)
project(helloworld)

set(CMAKE_CXX_STANDARD 17)

# Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "Vulkan found at ${Vulkan_LIBRARY}")
else()
    message(FATAL_ERROR "Vulkan could not be found")
endif()

# OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP for C++ found")# (version ${OpenMP_CXX_VERSION})")
else()
    message(FATAL_ERROR "OpenMP for C++ could not be found")
endif()

include_directories(
        inc
        Vulkan_INCLUDE_DIRS
)

set(ALL_SRCS

)

add_executable(HelloWorld
        ${ALL_SRCS}
        main.cpp
)

# TODO does everything here really needs to be public?
target_include_directories(HelloWorld
        PUBLIC inc
        PUBLIC ${Vulkan_Include_DIRS}
)

# TODO does everything here really needs to be public?
target_link_libraries(HelloWorld
        PUBLIC ${Vulkan_LIBRARIES}
        PUBLIC OpenMP::OpenMP_CXX
)