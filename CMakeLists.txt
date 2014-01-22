cmake_minimum_required(VERSION 2.8)
project(orc)

# Add all source files in the "src" directory
aux_source_directory(src SOURCES)
if (SOURCES STREQUAL "")
	message(FATAL_ERROR "No source files in the src directory")
endif()

###############################################################################
# luajit
###############################################################################
include(ExternalProject)

# Specify where to find and build the luajit source
set(luajit_path vendor/luajit)
ExternalProject_Add(luajit_src
	URL file://${CMAKE_CURRENT_SOURCE_DIR}/${luajit_path}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/${luajit_path}
  CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
  BUILD_COMMAND "make"
  INSTALL_COMMAND make install PREFIX="${CMAKE_CURRENT_BINARY_DIR}/${luajit_path}"
)

# Create a luajit target that depends on the luajit source
ExternalProject_Get_Property(luajit_src install_dir)
add_library(luajit STATIC IMPORTED)
set_property(TARGET luajit PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libluajit-5.1.a)
add_dependencies(luajit luajit_src)

include_directories(${install_dir}/include/luajit-2.0)


###############################################################################
# SDL
###############################################################################
# Specify where SDL is
include_directories("vendor/SDL/local/include/SDL2")
find_library(SDL_LIB SDL2 HINTS "vendor/SDL/local/lib")

# Build the orc executable using the found source files
add_executable(orc ${SOURCES})

# Link orc against the SDL library
target_link_libraries(orc ${SDL_LIB})

