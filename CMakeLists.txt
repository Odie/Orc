cmake_minimum_required(VERSION 2.8)
project(orc)

# Add all source files in the "src" directory
aux_source_directory(src SOURCES)
if (SOURCES STREQUAL "")
	message(FATAL_ERROR "No source files in the src directory")
endif()

# Specify where SDL is
include_directories("vendor/SDL/local/include/SDL2")
find_library(SDL_LIB SDL2 HINTS "vendor/SDL/local/lib")

# Build the orc executable using the found source files
add_executable(orc ${SOURCES})

# Link orc against the SDL library
target_link_libraries(orc ${SDL_LIB})

