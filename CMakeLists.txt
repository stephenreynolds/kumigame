cmake_minimum_required(VERSION 3.15)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

project(kumigame VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(CMAKE_CXX_STANDARD 20)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Doxygen.cmake)
include(cmake/StaticAnalyzers.cmake)

include(cmake/Conan.cmake)
run_conan()

add_executable(kumigame
    src/main.cpp
    src/game.cpp
    src/version.cpp
    src/settings.cpp
    src/camera.cpp
    src/vendor/stb_image.c
    src/debug/debugConsole.cpp
    src/debug/glDebug.cpp
    src/debug/log.cpp
    src/debug/statsViewer.cpp
    src/input/keyState.cpp
    src/input/keyboard.cpp
    src/renderer/texture.cpp
    src/renderer/mesh.cpp
    src/renderer/model.cpp
    src/renderer/shader.cpp
    src/renderer/textRenderer.cpp)

target_compile_definitions(kumigame PUBLIC
    -DRELEASE_TYPE="internal"
    -DVERSION_MAJOR=${VERSION_MAJOR}
    -DVERSION_MINOR=${VERSION_MINOR}
    -DVERSION_PATCH=${VERSION_PATCH}
    -DGLFW_INCLUDE_NONE)

target_link_libraries(kumigame
    PRIVATE
    CONAN_PKG::assimp
    CONAN_PKG::fmt
    CONAN_PKG::freetype
    CONAN_PKG::glad
    CONAN_PKG::glfw
    CONAN_PKG::glm
    CONAN_PKG::spdlog
    CONAN_PKG::stb
    CONAN_PKG::toml11)

file(GLOB_RECURSE assets RELATIVE ${CMAKE_SOURCE_DIR}/assets CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/assets/*)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Create settings.toml from sample if it does not already exist.
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/settings.toml)
    add_custom_command(TARGET kumigame
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/settings.toml.sample ${CMAKE_SOURCE_DIR}/settings.toml)
endif()

# Copy settings file to build directory.
add_custom_command(TARGET kumigame
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/settings.toml ${CMAKE_BINARY_DIR}/bin)
