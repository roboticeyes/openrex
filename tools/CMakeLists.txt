# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

set (rextrude_headers ${CMAKE_CURRENT_SOURCE_DIR}/rex-extrusion.h)

add_library(rextrude SHARED rex-extrusion.c)
target_link_libraries(rextrude openrex m)

if (STATICLIBS)
  add_library(rextrude-static STATIC rex-extrusion.c)
  set_target_properties(rextrude-static PROPERTIES OUTPUT_NAME "rextrude-static")
  set_target_properties(rextrude-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  install ( TARGETS rextrude-static
            ARCHIVE DESTINATION lib${LIB_SUFFIX}
            COMPONENT staticlibs
          )
endif()

install( TARGETS rextrude
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

install (FILES ${rextrude_headers}
    DESTINATION include/${PROJECT_NAME}
    COMPONENT headers
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

# Linkage
target_link_libraries(rex-dump openrex-static)
target_link_libraries(rex-extrude rextrude openrex-static m)
target_link_libraries(rex-info openrex-static)
target_link_libraries(rex-geojson openrex-static m)
target_link_libraries(rex-las openrex-static m)
target_link_libraries(rex-gen openrex-static)
target_link_libraries(rex-text openrex-static)
target_link_libraries(rex-asset openrex-static)

# Source includes
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

install( TARGETS rex-extrude rex-dump rex-info rex-gen rex-asset rex-las rex-text rex-geojson
    RUNTIME DESTINATION bin
    )
