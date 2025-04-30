# Build the Sandbox app as an executable
project(Sandbox)

file(GLOB_RECURSE SANDBOX_SRC CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

add_executable(Sandbox ${SANDBOX_SRC})

target_include_directories(Sandbox PRIVATE src)

target_compile_definitions(Sandbox PRIVATE TKC_PLATFORM_WINDOWS=1)

# Link against Tokucu
target_link_libraries(Sandbox PRIVATE TokucuEngine)

# Link extra third-party libs needed only by sandbox (if any)

