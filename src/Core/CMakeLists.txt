cmake_minimum_required(VERSION 3.1)
message("MINEMU Core")
project(Core C)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# List all the source files of the Core library
set(CORE_HEADERS
    include/minemu.h
    include/3rd/khash.h
    include/minemu/MNE_File.h
    include/minemu/MNE_Log.h
    include/minemu/MNE_Memory.h
    include/minemu/MNE_Flags.h)

set(CORE_SOURCES
    src/minemu.c
    src/minemu/MNE_Log.c
    src/minemu/MNE_File.c)


# Create the Core static library
add_library(Core STATIC ${CORE_HEADERS} ${CORE_SOURCES})

include_directories(include)

# Include directories for Core
target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)