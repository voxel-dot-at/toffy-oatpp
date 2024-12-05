


add_library(toffy-oatpp SHARED
    globals.cpp
    webapi/controllers/btaAdapterController.cpp
    webapi/controllers/webAdapterController.cpp
    webapi/filters/webAdapter.cpp
    webapi/filters/webListener.cpp
    webapi/webApp.cpp
    )

target_link_libraries(toffy-oatpp ${LIBS})

install(TARGETS toffy-oatpp DESTINATION lib)