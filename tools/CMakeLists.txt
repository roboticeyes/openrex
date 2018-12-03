# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/extensions
    )

# Targets
add_executable(rex-dump rex-dump.c)
add_executable(rex-extrude rex-extrude.c)
add_executable(rex-info rex-info.c)
add_executable(rex-geojson rex-geojson.c cJSON.c)
add_executable(rex-las rex-las.c)
add_executable(rex-gen rex-gen.c)
add_executable(rex-text rex-text.c)
add_executable(rex-asset rex-asset.c)

if(UNIX)
	set(MLIB m)
endif()

# Linkage
target_link_libraries(rex-dump openrex-static)
target_link_libraries(rex-extrude rextension-static openrex-static m)
target_link_libraries(rex-info openrex-static)
target_link_libraries(rex-extrude openrex-static ${MLIB})
target_link_libraries(rex-geojson openrex-static ${MLIB})
target_link_libraries(rex-las openrex-static ${MLIB})
target_link_libraries(rex-gen openrex-static)
target_link_libraries(rex-text openrex-static)
target_link_libraries(rex-asset openrex-static)

install( TARGETS rex-extrude rex-dump rex-info rex-gen rex-asset rex-las rex-text rex-geojson
    RUNTIME DESTINATION bin
    )
