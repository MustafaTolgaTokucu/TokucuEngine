# Build the Tokucu engine as a static library
project(TokucuEngine)

file(GLOB_RECURSE TOKUCUENGINE_SRC CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

file(GLOB_RECURSE VENDOR_SRC CONFIGURE_DEPENDS
    "vendor*.cpp"
    "vendor*.h"
    "vendor*.c"
)

# Organize Visual Studio folders (optional but pro-looking)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${TOKUCUENGINE_SRC})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/vendor" PREFIX "Vendor" FILES ${VENDOR_SRC})

add_library(TokucuEngine STATIC ${TOKUCUENGINE_SRC})

target_include_directories(TokucuEngine PUBLIC src)
target_compile_definitions(TokucuEngine PRIVATE TKC_PLATFORM_WINDOWS=1)

#third-party dependencies
target_link_libraries(TokucuEngine
    PUBLIC glfw
    PUBLIC assimp
)

# Include your engine and vendor headers
target_include_directories(TokucuEngine PUBLIC
    src
    vendor
)

# Precompiled header
target_precompile_headers(TokucuEngine PRIVATE src/tkcpch.h)
set_source_files_properties(glad.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)