


add_library(toffy_oatpp SHARED
    globals.cpp
    webapi/controllers/btaAdapterController.cpp
    filters/webAdapter.cpp
    filters/webListener.cpp
    webapi/webApp.cpp
    )

target_link_libraries( toffy_oatpp PUBLIC    ${LIBS}  )
set_property(TARGET toffy_oatpp PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET toffy_oatpp PROPERTY SOVERSION ${PROJECT_VERSION})
install (TARGETS toffy_oatpp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

