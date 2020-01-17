cmake_minimum_required(VERSION 3.14)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OUTPUT_DEST "${CMAKE_BINARY_DIR}/kumigame")

project(kumigame VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("deps/glfw-3.3")

# GLAD
set(GLAD_INCLUDE_DIR "deps/glad/include")
set(GLAD_SOURCE_DIR "deps/glad/src")
set(GLAD_SOURCES
    ${GLAD_INCLUDE_DIR}/glad/glad.h
    ${GLAD_INCLUDE_DIR}/KHR/khrplatform.h
    ${GLAD_SOURCE_DIR}/glad.c)

# spdlog
set(SPDLOG_BUILD_EXAMPLES OFF)
add_subdirectory("deps/spdlog-1.4.2")

# fmt
set(FMT_TEST FALSE)
add_subdirectory("deps/fmt-6.0.0")

# glm
set(GLM_INCLUDE_DIR "deps/glm")
add_subdirectory("deps/glm")

# Freetype
add_subdirectory("deps/freetype-2.10.0")

# cpptoml
add_subdirectory("deps/cpptoml-0.1.1")
set(CPPTOML_INCLUDE_DIR "deps/cpptoml-0.1.1/include")

# stb
set(STB_INCLUDE_DIR "deps/stb-master")

include_directories(
    "include"
    ${GLAD_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${CPPTOML_INCLUDE_DIR}
    ${STB_INCLUDE_DIR})

set(PROJECT_SOURCES
    ${GLAD_SOURCES}
    src/main.cpp
    include/debug/log.hpp
    src/debug/log.cpp
    include/settings.hpp
    src/settings.cpp
    include/debug/glDebug.hpp
    src/debug/glDebug.cpp
    include/game.hpp
    src/game.cpp
    include/version.hpp
    src/version.cpp include/renderer/textRenderer.hpp
    src/renderer/textRenderer.cpp
    include/renderer/shader.hpp
    src/renderer/shader.cpp
    include/debug/statsViewer.hpp
    src/debug/statsViewer.cpp
    include/input/keyState.hpp
    include/input/keyboard.hpp
    src/input/keyState.cpp
    src/input/keyboard.cpp
    include/renderer/texture.hpp
    src/renderer/texture.cpp
    src/vendor/stb_image.c src/camera.cpp include/camera.hpp)

add_executable(kumigame ${PROJECT_SOURCES})

set_target_properties(kumigame PROPERTIES
    OUTPUT_NAME "kumigame"
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DEST})

target_compile_definitions(kumigame PUBLIC
    -DRELEASE_TYPE="internal"
    -DVERSION_MAJOR=${VERSION_MAJOR}
    -DVERSION_MINOR=${VERSION_MINOR}
    -DVERSION_PATCH=${VERSION_PATCH}
    -DGLFW_INCLUDE_NONE)

target_compile_options(kumigame PRIVATE -Wall -Wextra)

target_compile_features(kumigame PRIVATE cxx_std_17)

target_link_libraries(kumigame PRIVATE
    glfw
    spdlog
    fmt::fmt
    freetype)

# Copy assets to build directory.
set(ASSET_DIRECTORIES
    fonts
    shaders
    textures)

foreach(ASSET_DIR ${ASSET_DIRECTORIES})
    add_custom_command(TARGET kumigame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/${ASSET_DIR} ${OUTPUT_DEST}/assets/${ASSET_DIR})
endforeach()

# Copy settings file to build directory
add_custom_command(TARGET kumigame
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/settings.toml ${OUTPUT_DEST})