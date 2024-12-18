


add_library(toffy-oatpp SHARED
    globals.cpp
    webapi/controllers/btaAdapterController.cpp
    filters/webAdapter.cpp
    filters/webListener.cpp
    webapi/webApp.cpp
    )

target_link_libraries( toffy-oatpp PUBLIC    ${LIBS}  )
set_property(TARGET toffy-oatpp PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET toffy-oatpp PROPERTY SOVERSION ${PROJECT_VERSION})
install (TARGETS toffy-oatpp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
