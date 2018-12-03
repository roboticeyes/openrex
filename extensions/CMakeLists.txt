# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

set (rextension_headers ${CMAKE_CURRENT_SOURCE_DIR}/rex-extrusion.h)

add_library(rextension SHARED rex-extrusion.c)

if (STATICLIBS)
  add_library(rextension-static STATIC rex-extrusion.c)
  set_target_properties(rextension-static PROPERTIES OUTPUT_NAME "rextension-static")
  set_target_properties(rextension-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  install ( TARGETS rextension-static
            ARCHIVE DESTINATION lib${LIB_SUFFIX}
            COMPONENT staticlibs
          )

  target_link_libraries(rextension openrex-static)
else()
  target_link_libraries(rextension openrex)
endif()

install( TARGETS rextension
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

install (FILES ${rextension_headers}
    DESTINATION include/${PROJECT_NAME}
    COMPONENT headers
    )

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)