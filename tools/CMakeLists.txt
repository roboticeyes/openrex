# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# Targets
add_executable(rex-info rex-info.c)
add_executable(rex-las rex-las.c)
# add_executable(rex-merge rex-merge.c)
add_executable(rex-gen rex-gen.c)
add_executable(rex-text rex-text.c)
add_executable(rex-asset rex-asset.c)

# Linkage
target_link_libraries(rex-info openrex-static)
target_link_libraries(rex-las openrex-static m)
# target_link_libraries(rex-merge openrex-static)
target_link_libraries(rex-gen openrex-static)
target_link_libraries(rex-text openrex-static)
target_link_libraries(rex-asset openrex-static)

# Source includes
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

install( TARGETS rex-info rex-gen rex-asset rex-las rex-text
    RUNTIME DESTINATION bin
    )

