cmake_minimum_required(VERSION 3.16)
project(TinyRT C)

set(CMAKE_C_STANDARD 11)

# GLFW options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add GLFW
add_subdirectory(thirdparty/glfw)

# Add GLAD
add_subdirectory(thirdparty/glad)

# Add cJSON
add_subdirectory(thirdparty/cjson)

# Source files
file(GLOB_RECURSE SOURCES src/*.c)

# Header files
file(GLOB_RECURSE HEADERS src/*.h)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
    cJSON
)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    thirdparty/glfw/include
    thirdparty/glad/include
    thirdparty/cjson/include
)
