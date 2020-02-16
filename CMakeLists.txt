cmake_minimum_required(VERSION 3.14)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OUTPUT_DEST "${CMAKE_BINARY_DIR}/kumigame")

project(kumigame VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

include(GetLibraries.cmake)

set(PROJECT_SOURCES
    src/main.cpp
    src/vendor/stb_image.c
    include/debug/log.hpp
    src/debug/log.cpp
    include/settings.hpp
    src/settings.cpp
    include/debug/glDebug.hpp
    src/debug/glDebug.cpp
    include/game.hpp
    src/game.cpp
    include/version.hpp
    src/version.cpp
    include/renderer/textRenderer.hpp
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
    src/camera.cpp
    include/camera.hpp
    include/debug/debugConsole.hpp
    src/debug/debugConsole.cpp
    include/renderer/mesh.hpp
    src/renderer/mesh.cpp
    include/renderer/model.hpp
    src/renderer/model.cpp)

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

target_compile_features(kumigame PRIVATE cxx_std_20)

target_include_directories(kumigame
    PRIVATE
    include
    ${CMAKE_BINARY_DIR}/external/installed/include)

target_link_libraries(kumigame
    assimp
    fmt
    freetype
    glad
    glfw
    glm
    spdlog)

file(GLOB_RECURSE assets RELATIVE ${CMAKE_SOURCE_DIR}/assets CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/assets/*)
file(COPY assets DESTINATION ${OUTPUT_DEST})

# Copy settings file to build directory
add_custom_command(TARGET kumigame
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/settings.toml ${OUTPUT_DEST})
