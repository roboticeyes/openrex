# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# Targets
add_executable(rex-info rex-info.c)
add_executable(rex-gencube rex-gencube.c)


# Linkage
target_link_libraries(rex-info openrex)
target_link_libraries(rex-gencube openrex)

# Source includes
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

install( TARGETS rex-info rex-gencube
    RUNTIME DESTINATION bin
    )

