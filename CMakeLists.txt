cmake_minimum_required(VERSION 3.14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(OUTPUT_DEST "${CMAKE_BINARY_DIR}/kumigame")

project(kumigame VERSION 0.0.1)

# Detect target OS
if(WIN32)
    set(OS_TARGET win)
    message(STATUS "Targeting Windows")
elseif(APPLE)
    set(OS_TARGET mac)
    message(STATUS "Targeting macOS")
else()
    set(OS_TARGET linux)
    message(STATUS "Targeting Linux")
endif()

# Set compiler options
if(MSVC)
    # MSVC
    message(STATUS "Compiling with MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Ob0 /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -static-libgcc -static-libstdc++ -Wall -Wextra -isystem ${CMAKE_CURRENT_BINARY_DIR}/include")
    set(CMAKE_EXE_LINKER_FLAGS -lstdc++fs)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # Clang
        message(STATUS "Compiling with Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    else()
        # GCC
        message(STATUS "Compiling with GCC")
    endif()
endif()

# Set library files
if(MSVC)
    link_directories(
        "deps/libs/msvc/x64"
    )

    set(ASSIMP_LIB assimp-vc140-mt.lib)
    set(FMT_LIB fmt.lib)
    set(FREETYPE_LIB freetype.lib)
    set(GLFW_LIB glfw3.lib)
    set(GLM_LIB glm_static.lib)
else()
    # Only MSVC is supported now.
    message(FATAL_ERROR "The project must be compiled with MSVC!")
endif()

include_directories(
    "include"
    "deps/include"
)

set(PROJECT_HEADERS
    include/game.hpp
    include/result.hpp
    include/settings.hpp
    include/shader.hpp
    include/textRenderer.hpp
    include/version.hpp
    include/debug/glDebug.hpp
    include/debug/log.hpp
    include/debug/statsViewer.hpp
    include/input/keyboard.hpp
    include/input/keyState.hpp
)

set(PROJECT_SOURCES
    src/main.cpp
    src/game.cpp
    src/settings.cpp
    src/shader.cpp
    src/textRenderer.cpp
    src/version.cpp
    src/debug/glDebug.cpp
    src/debug/log.cpp
    src/debug/statsViewer.cpp
    src/input/keyboard.cpp
    src/input/keyState.cpp
    src/vendor/glad.c
    src/vendor/stb_image.c
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "kumigame"
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DEST}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    -DRELEASE_TYPE="internal"
    -DGLFW_INCLUDE_NONE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ASSIMP_LIB}
    ${FMT_LIB}
    ${FREETYPE_LIB}
    ${GLFW_LIB}
    ${GLM_LIB}
)

# Copy assets to build directory.
set(ASSET_DIRECTORIES
    fonts
    shaders
)

foreach(ASSET_DIR ${ASSET_DIRECTORIES})
  add_custom_command(TARGET ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/${ASSET_DIR} ${OUTPUT_DEST}/assets/${ASSET_DIR})
endforeach()

# Copy settings file to build directory
add_custom_command(TARGET ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/settings.toml ${OUTPUT_DEST})

# Copy shared libraries to build directory.
if(MSVC)
    set(SHARED_LIBS
        assimp-vc140-mt.dll
    )
    foreach(SHARED_LIB ${SHARED_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/deps/bin/${SHARED_LIB} ${OUTPUT_DEST})
    endforeach()
else()
    # Only MSVC is supported now.
    message(FATAL_ERROR "The project must be compiled with MSVC!")
endif()
