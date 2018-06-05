set(c_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-header.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-image.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-lineset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-material.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-mesh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-pointlist.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-text.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-unitypackage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/util.c
    )

set(c_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-header.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-lineset.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-pointlist.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-text.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex-block-unitypackage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/linmath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/list.h
    ${CMAKE_CURRENT_SOURCE_DIR}/status.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util.h
    CACHE INTERNAL "List of c sources" )

add_library(openrex SHARED ${c_sources})

install( TARGETS openrex
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

if (STATICLIBS)
  add_library(openrex-static STATIC ${c_sources})
  set_target_properties(openrex-static PROPERTIES OUTPUT_NAME "openrex-static")
  set_target_properties(openrex-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  install ( TARGETS openrex-static
            ARCHIVE DESTINATION lib${LIB_SUFFIX}
            COMPONENT staticlibs
          )
endif()

install (FILES ${c_headers}
    DESTINATION include/${PROJECT_NAME}
    COMPONENT headers
    )
