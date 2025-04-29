# Build the Tokucu engine as a static library
project(TokucuEngine)

file(GLOB_RECURSE TOKUCUENGINE_SRC CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

file(GLOB_RECURSE VENDOR_SRC CONFIGURE_DEPENDS
    "vendor/imgui/*.cpp"
)

# Organize Visual Studio folders (optional but pro-looking)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${TOKUCU_SRC})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/vendor" PREFIX "Vendor" FILES ${VENDOR_SRC})

add_library(TokucuEngine STATIC ${TOKUCUENGINE_SRC})

target_include_directories(TokucuEngine PUBLIC src)

# Link third-party libraries here if the engine needs them
# Example:
target_link_libraries(TokucuEngine PUBLIC GLFW)
target_link_libraries(TokucuEngine PUBLIC GLAD)
target_link_libraries(TokucuEngine PUBLIC Assimp)

# Include your engine and vendor headers
target_include_directories(Tokucu PUBLIC
    src
    vendor
)

# Precompiled header
target_precompile_headers(Tokucu PRIVATE src/tkcpch.h)