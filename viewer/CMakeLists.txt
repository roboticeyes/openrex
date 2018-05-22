# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Target
add_executable(rex-view
    camera.c
    rex-view.c
    shader.c
    mesh.c
    mesh_group.c
    )

# Linkage
target_link_libraries(rex-view
    openrex-static
    SDL2
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    m
    )

# Source includes
include_directories(rex-view
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    )

install(TARGETS rex-view
    RUNTIME DESTINATION bin)
