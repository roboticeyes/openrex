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

if(UNIX)
	set(MLIB m)
endif()

# Linkage
if (STATICLIBS)
  target_link_libraries(rex-dump openrex-static)
  target_link_libraries(rex-extrude rextension-static openrex-static ${MLIB})
  target_link_libraries(rex-info openrex-static)
  target_link_libraries(rex-geojson openrex-static ${MLIB})
  target_link_libraries(rex-las openrex-static ${MLIB})
  target_link_libraries(rex-gen openrex-static)
  target_link_libraries(rex-text openrex-static)
else()
  target_link_libraries(rex-dump openrex)
  target_link_libraries(rex-extrude rextension openrex ${MLIB})
  target_link_libraries(rex-info openrex)
  target_link_libraries(rex-geojson openrex ${MLIB})
  target_link_libraries(rex-las openrex ${MLIB})
  target_link_libraries(rex-gen openrex)
  target_link_libraries(rex-text openrex)
endif()

install( TARGETS rex-extrude rex-dump rex-info rex-gen rex-las rex-text rex-geojson
    RUNTIME DESTINATION bin
    )
