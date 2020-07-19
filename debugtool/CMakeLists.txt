set(DBT_LIBRARIES debuglib)
set(DBT_INCLUDE_DIRS inc)

add_library(${DBT_LIBRARIES} STATIC
    src/to_be_removed.cpp
)

# TODO does everything here really needs to be public?
target_include_directories(${DBT_LIBRARIES}
        PUBLIC ${DBT_INCLUDE_DIRS}
        PUBLIC ${Vulkan_INCLUDE_DIRS}
)

# TODO does everything here really needs to be public?
target_link_libraries(${DBT_LIBRARIES}
        PUBLIC ${Vulkan_LIBRARIES}
)