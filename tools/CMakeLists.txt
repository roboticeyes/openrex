# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# Targets
add_executable(rex-info rex-info.c)
add_executable(rex-gencube rex-gencube.c)
add_executable(rex-asset rex-asset.c)
add_executable(rex-view rex-view.c)


# Linkage
target_link_libraries(rex-info openrex-static)
target_link_libraries(rex-gencube openrex-static)
target_link_libraries(rex-asset openrex-static)
target_link_libraries(rex-view openrex-static SDL2 GL GLEW)

# Source includes
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

install( TARGETS rex-info rex-gencube rex-asset
    RUNTIME DESTINATION bin
    )

