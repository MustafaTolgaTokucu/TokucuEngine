cmake_minimum_required(VERSION 3.15)
project(TokucuEngine)

set(CMAKE_CXX_STANDARD 20)

# Prevent GLFW from building examples/tests/docs
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

# ⬇️ Choose static or dynamic
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)  # OFF = Static, ON = Shared DLL
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)  # Set to ON for DLL version

# Add 3rd party lib
add_subdirectory(dependencies/glfw)

# Find existing Assimp installation instead of building it
# Option 1: Using find_package for a pre-installed Assimp
find_package(assimp QUIET)

if(NOT assimp_FOUND)
    # Option 2: If not found, use FetchContent to download and configure Assimp
    # but not build it as part of the main project
    include(FetchContent)
    
    # Configure Assimp options
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
    
    # Use FetchContent to get Assimp
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.2.5  # Use a specific version tag
    )
    
    # This makes the Assimp targets available without building as part of ALL_BUILD
    FetchContent_MakeAvailable(assimp)
endif()

# Add subdirectories
add_subdirectory(TokucuEngine)
add_subdirectory(Sandbox)