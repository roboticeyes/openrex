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
    axis.c
    camera.c
    gameengine.c
    polyline.c
    mesh.c
    mesh_group.c
    points.c
    rex-view.c
    material.c
    scene.c
    shader.c
    )

set(shaders
    shaders/mesh.fs
    shaders/mesh.vs
    shaders/pointcloud.fs
    shaders/pointcloud.vs
    shaders/polyline.fs
    shaders/polyline.vs
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


install (FILES ${shaders}
    DESTINATION share/${PROJECT_NAME}
    )

