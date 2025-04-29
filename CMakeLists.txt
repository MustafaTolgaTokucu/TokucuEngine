cmake_minimum_required(VERSION 3.15)
project(TokucuEngine)

set(CMAKE_CXX_STANDARD 20)

# Prevent GLFW from building examples/tests/docs
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

# Disable unnecessary components
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)  # Optional: if you don’t need export functionality
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # Avoid external zlib dependency

# ⬇️ Choose static or dynamic
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)  # OFF = Static, ON = Shared DLL
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)  # Set to ON for DLL version

# Add 3rd party lib
add_subdirectory(dependencies/glfw)
add_subdirectory(Dependencies/GLAD)
add_subdirectory(Dependencies/Assimp)

# Add subdirectories
add_subdirectory(TokucuEngine)
add_subdirectory(Sandbox)