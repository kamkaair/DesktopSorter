# Specify minimum supported cmake version
cmake_minimum_required(VERSION 3.5)

# set the project name
project(DesktopSorter)

# Find all header files
file(GLOB_RECURSE HDR_FILES "src/*.h")
source_group("Header Files", FILES ${HDR_FILES})

# DISABLED FOR NOW!!
# Set the assets directory relative to the project root
#set(ASSET_DIR "${CMAKE_SOURCE_DIR}/assets")

# Add this as a preprocessor definition
#add_compile_definitions(ASSET_DIR="${ASSET_DIR}")
# DISABLED FOR NOW!!

# Find all source files in the src folder
# file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
source_group("Source Files", FILES ${SRC_FILES})

# add the executables
add_executable(DesktopSorter ${HDR_FILES} ${SRC_FILES})
