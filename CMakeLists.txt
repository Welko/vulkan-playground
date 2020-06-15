cmake_minimum_required(VERSION 3.15)
project(helloworld)

set(CMAKE_CXX_STANDARD 17)

# GLM sets
set(CMAKE_EXE_LINKER_FLAGS "-static") # Dunno exactly what it does. TODO: find out
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/glm/cmake/glm) # TODO make easier to customize

# GLFW sets
# TODO: don't do it manually...
set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw-3.3.2.bin.WIN64")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
set(GLFW_LIBRARIES
        "${GLFW_DIR}/lib-mingw-w64/glfw3.dll"
        "${GLFW_DIR}/lib-mingw-w64/libglfw3.a"
        "${GLFW_DIR}/lib-mingw-w64/libglfw3dll.a"
        )
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
message(STATUS "GLM root dir manually set to ${GLM_DIR}")
find_package(GLM REQUIRED)
if(GLM_FOUND)
    message(STATUS "GLM found. Include dirs: ${GLM_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "GLM could not be found")
endif()

# GLFW (Graphics Library Framework) - for window management
message(STATUS "GLFW dir manually set to ${GLFW_DIR}")
message(STATUS "GLFW include dir manually set to ${GLFW_INCLUDE_DIR}")
message(STATUS "GLFW libraries manually set to ${GLFW_LIBRARIES}")

add_executable(HelloWorld
        main.cpp


)

# TODO does everything here really needs to be public?
target_include_directories(HelloWorld
        PUBLIC inc
        PUBLIC ${Vulkan_INCLUDE_DIRS}
        PUBLIC ${GLM_INCLUDE_DIRS}
        PUBLIC ${GLFW_INCLUDE_DIR}
)

# TODO does everything here really needs to be public?
target_link_libraries(HelloWorld
        PUBLIC ${Vulkan_LIBRARIES}
        PUBLIC ${GLFW_LIBRARIES}
        PUBLIC OpenMP::OpenMP_CXX
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug mode on")
    add_definitions("-DM_DEBUG")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Release mode on")
endif()