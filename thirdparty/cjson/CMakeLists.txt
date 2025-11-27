cmake_minimum_required(VERSION 3.16)

project(cJSON C)

# Create the cJSON library
add_library(cJSON STATIC
    src/cJSON.c
    include/cjson/cJSON.h
)

# Set include directories for the library
target_include_directories(cJSON PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C standard
set_property(TARGET cJSON PROPERTY C_STANDARD 99)
set_property(TARGET cJSON PROPERTY C_STANDARD_REQUIRED ON)
