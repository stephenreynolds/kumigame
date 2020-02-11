cmake_minimum_required(VERSION 3.14)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OUTPUT_DEST "${CMAKE_BINARY_DIR}/kumigame")

project(kumigame VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

include(FetchContent)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.2
)
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.33
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    set(GLAD_PROFILE "core")
    set(GLAD_API "gl=4.3")
    set(GLAD_GENERATOR "c")
    set(GLAD_EXTENSIONS "")
    set(GLAD_SPEC "gl")
    set(GLAD_NO_LOADER OFF)
    set(GLAD_REPRODUCIBLE ON)
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
    set(GLAD_INCLUDE_DIR ${glad_BINARY_DIR}/include)
endif()

# spdlog
set(SPDLOG_BUILD_EXAMPLES OFF)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.5.0
)
FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

# fmt
set(FMT_TEST FALSE)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 6.1.2
)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()

# glm
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.7
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
    set(GLM_INCLUDE_DIR ${glm_SOURCE_DIR}/glm)
endif()

# Freetype
FetchContent_Declare(
    freetype
    URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.0.tar.gz
)
FetchContent_GetProperties(freetype)
if(NOT freetype_POPULATED)
    FetchContent_Populate(freetype)
    add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR})
endif()

# cpptoml
FetchContent_Declare(
    cpptoml
    GIT_REPOSITORY https://github.com/skystrife/cpptoml.git
    GIT_TAG v0.1.1
)
FetchContent_GetProperties(cpptoml)
if(NOT cpptoml_POPULATED)
    FetchContent_Populate(cpptoml)
    add_subdirectory(${cpptoml_SOURCE_DIR} ${cpptoml_BINARY_DIR})
    set(CPPTOML_INCLUDE_DIR ${cpptoml_SOURCE_DIR}/include)
endif()

# Assimp
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.0.1
)
FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
    FetchContent_Populate(assimp)
    add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
endif()

# stb
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
    set(STB_INCLUDE_DIR ${stb_SOURCE_DIR})
endif()

# strutil
FetchContent_Declare(
    strutil
    GIT_REPOSITORY https://github.com/Shot511/strutil.git
)
FetchContent_GetProperties(strutil)
if(NOT strutil_POPULATED)
    FetchContent_Populate(strutil)
    set(STRUTIL_INCLUDE_DIR ${strutil_SOURCE_DIR})
endif()

include_directories(
    "include"
    ${GLAD_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${CPPTOML_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    ${STRUTIL_INCLUDE_DIR})

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

target_link_libraries(kumigame PRIVATE
    glfw
    glad
    spdlog
    fmt::fmt
    freetype
    assimp)

file(GLOB_RECURSE assets RELATIVE ${CMAKE_SOURCE_DIR}/assets CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/assets/*)
file(COPY assets DESTINATION ${OUTPUT_DEST})

# Copy settings file to build directory
add_custom_command(TARGET kumigame
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/settings.toml ${OUTPUT_DEST})
