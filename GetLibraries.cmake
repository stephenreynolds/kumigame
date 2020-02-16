include(ExternalProject)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external/installed/include/)

add_custom_target(external_all)
set_target_properties(external_all PROPERTIES EXCLUDE_FROM_ALL TRUE)

# Assimp
ExternalProject_Add(assimp_external
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.0.1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=ON
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
        -DASSIMP_BUILD_TESTS=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installed)
set_target_properties(assimp_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all assimp_external)

add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/installed/lib/libassimp.a)
target_link_libraries(assimp INTERFACE irrXML z)

add_library(irrXML STATIC IMPORTED)
set_target_properties(irrXML PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/installed/lib/libIrrXML.a)

# cpptoml
ExternalProject_Add(cpptoml_external
    GIT_REPOSITORY https://github.com/skystrife/cpptoml.git
    GIT_TAG v0.1.1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installed)
set_target_properties(cpptoml_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all cpptoml_external)

# fmt
ExternalProject_Add(fmt_external
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 6.1.2
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installed)
set_target_properties(fmt_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all fmt_external)

add_library(fmt STATIC IMPORTED)
set_target_properties(fmt PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/installed/lib/libfmt.a)

# Freetype
ExternalProject_Add(freetype_external
    URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.0.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installed)
set_target_properties(freetype_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all freetype_external)

add_library(freetype STATIC IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/installed/lib/libfreetype.a)
target_link_libraries(freetype INTERFACE harfbuzz png bz2)

add_custom_command(TARGET freetype_external POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_BINARY_DIR}/external/installed/include/freetype2 ${CMAKE_CURRENT_BINARY_DIR}/external/installed/include)

# GLAD
ExternalProject_Add(glad_external
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.33
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DGLAD_PROFILE="core"
        -DGLAD_API="gl=4.3"
        -DGLAD_GENERATOR="c"
        -DGLAD_EXTENSIONS=""
        -DGLAD_SPEC="gl"
        -DGLAD_NO_LOADER="OFF"
        -DGLAD_REPRODUCIBLE="ON"
        -DGLAD_INSTALL="ON"
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installed)
set_target_properties(glad_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all glad_external)

add_library(glad STATIC IMPORTED)
set_target_properties(glad PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/installed/lib/libglad.a)

# GLFW
ExternalProject_Add(glfw_external
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.2
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installed)
set_target_properties(glfw_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all glfw_external)

add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/installed/lib/libglfw3.a)

# glm
ExternalProject_Add(glm_external
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.7
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installe
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/external/src/glm_external/glm ${CMAKE_CURRENT_BINARY_DIR}/external/installed/include/glm)
set_target_properties(glm_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all glm_external)

add_library(glm STATIC IMPORTED)
set_target_properties(glm PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/src/glm_external-build/glm/libglm_static.a)

# spdlog
ExternalProject_Add(spdlog_external
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.5.0
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/installed)
set_target_properties(spdlog_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all spdlog_external)

add_library(spdlog STATIC IMPORTED)
set_target_properties(spdlog PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/installed/lib/libspdlog.a)

# stb
set(STB_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/external/src/stb_external/stb_image.h)
ExternalProject_Add(stb_external
    GIT_REPOSITORY https://github.com/nothings/stb.git
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${STB_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/external/installed/include)
set_target_properties(stb_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all stb_external)

# strutil
ExternalProject_Add(strutil_external
    GIT_REPOSITORY https://github.com/Shot511/strutil.git
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/external/src/strutil_external/strutil.h ${CMAKE_CURRENT_BINARY_DIR}/external/installed/include)
set_target_properties(strutil_external PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(external_all strutil_external)
