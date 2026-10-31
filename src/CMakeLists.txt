


set(TOFFY_OATPP_SOURCES
    globals.cpp
    toffy/webAdapter.cpp
    webapi/webApi.cpp
    )

# The BTA adapter is a translation unit, not a line of code: it is the only thing
# that includes toffy/bta/*. Excluded when the toffy being built against has no
# BTA (P1-13) — see the flag and the endpoint decision in the top-level CMakeLists.
if (TOFFYOATPP_WITH_BTA)
    list(APPEND TOFFY_OATPP_SOURCES webapi/controllers/btaAdapterController.cpp)
endif()

add_library(toffy_oatpp SHARED ${TOFFY_OATPP_SOURCES})

target_link_libraries( toffy_oatpp PUBLIC    ${LIBS}  )
# 0 or 1, always defined, PUBLIC so it travels with the exported target: an
# application's `#if TOFFYOATPP_WITH_BTA` then describes the library it is linking,
# not the build directory it happens to be standing in. Same rule toffy now applies
# to TOFFY_WITH_BTA.
target_compile_definitions(toffy_oatpp PUBLIC "TOFFYOATPP_WITH_BTA=${TOFFYOATPP_WITH_BTA}")
set_property(TARGET toffy_oatpp PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET toffy_oatpp PROPERTY SOVERSION ${PROJECT_VERSION})
install (TARGETS toffy_oatpp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

