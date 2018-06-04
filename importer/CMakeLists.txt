# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

find_package(assimp REQUIRED)

# Target
add_executable(rex-importer
    rex-importer.c
    )

# Linkage
target_link_libraries(rex-importer
    openrex-static
    ${ASSIMP_LIBRARIES}
    m
    )

# Source includes
include_directories(rex-importer
    ${ASSIMP_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    )

install(TARGETS rex-importer
    RUNTIME DESTINATION bin)
