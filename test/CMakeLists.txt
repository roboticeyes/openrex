set(TEST_DATA_PATH ${CMAKE_SOURCE_DIR}/data)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# Source includes
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

add_definitions(-DTEST_DATA_PATH=\"${TEST_DATA_PATH}\")

set(c_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_all.c
    )

add_executable(test_all
    ${c_sources}
    )

add_test(
    NAME test_all
    COMMAND test_all
    )

target_link_libraries(test_all
    openrex-static
    check
    )
