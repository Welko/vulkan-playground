cmake_minimum_required(VERSION 3.15)
project(helloworld)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
#message(STATUS "Cmake module path set to ${CMAKE_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext")
message(${CMAKE_MODULE_PATH})

# Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "Vulkan found at ${Vulkan_LIBRARY}. Include dirs: ${Vulkan_INCLUDE_DIRS}")
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

# GLM (OpenGL Mathematics)
set(GLM_DIR ${CMAKE_MODULE_PATH}/glm/cmake/glm) # TODO make easier to customize
message(STATUS "GLM root dir set to ${GLM_DIR}")
find_package(GLM REQUIRED)
if(GLM_FOUND)
    message(STATUS "GLM found. Include dirs: ${GLM_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "GLM could not be found")
endif()

# GLFW (Graphics Library Framework) - for window management
# GLFW will be built from source! (Source 12/06/2020: https://www.glfw.org/docs/3.3/build_guide.html#build_link_cmake_source)
# TODO find better/easier way
    # Disable tests, examples and documentation
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    # Add subdirectory of the source tree
    add_subdirectory(${CMAKE_MODULE_PATH}/glfw-3.3.2) # TODO make easier to customize
    # Now we just need to link the library and that's it!

set(ALL_SRCS

)

add_executable(HelloWorld
        ${ALL_SRCS}
        main.cpp
)

# TODO does everything here really needs to be public?
target_include_directories(HelloWorld
        PUBLIC inc
        PUBLIC ${Vulkan_INCLUDE_DIRS}
        PUBLIC ${GLM_INCLUDE_DIRS}
)

# TODO does everything here really needs to be public?
target_link_libraries(HelloWorld
        PUBLIC ${Vulkan_LIBRARIES}
        PUBLIC glfw
        PUBLIC OpenMP::OpenMP_CXX
)