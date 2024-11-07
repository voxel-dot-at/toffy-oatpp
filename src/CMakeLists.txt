


add_library(toffy-oatpp 
    globals.cpp
    webapi/controllers/btaAdapterController.cpp
    webapi/controllers/webAdapterController.cpp
    )

target_link_libraries(toffy-oatpp ${LIBS})