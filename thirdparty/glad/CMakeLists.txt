cmake_minimum_required(VERSION 3.16)

project(glad LANGUAGES C)

# Create GLAD library
add_library(glad STATIC
    src/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
)

# Specify include directories
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C standard
set_property(TARGET glad PROPERTY C_STANDARD 11)
