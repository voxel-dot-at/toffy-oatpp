
#include <iostream>
#include <thread>

#include "oatpp/network/Server.hpp"

#include "oatpp-swagger/Controller.hpp"

#include "webapi/controller/staticContentsController.hpp"
#include "webapi/controller/btaAdapterController.hpp"
#include "filters/webAdapter.hpp"
#include "webapi/webAppComponent.hpp"
#include "webapi/swaggerComponent.hpp"

#include "globals.hpp"


static oatpp::network::Server* theServer = nullptr;
static std::thread* runner = nullptr;
static toffy::webapi::WebAdapter* webAdap = nullptr;
static SystemState* systemState = nullptr;

using namespace toffy::webapi;

namespace toffy {
namespace webapi {

static void run()
{
    /* Register Components in scope of run() method */
    AppComponent components;

    /* Get router component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    oatpp::web::server::api::Endpoints docEndpoints;
    /***
     * REGISTER CONTORLLERS
     */


    // auto wBoxCtrl = std::make_shared<WBoxController>();
    // router->addController(wBoxCtrl);
    // docEndpoints.append(wBoxCtrl->getEndpoints());

    // if (theState.enableSwagger) { ...
    router->addController(oatpp::swagger::Controller::createShared(docEndpoints));
    // ...}

    // static contents:
    router->addController(std::make_shared<StaticContentsController>());

    /***
     * /REGISTER CONTORLLERS
     */

    /* Get connection handler component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                    connectionHandler);

    /* Get connection provider component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>,
                    connectionProvider);

    /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
    oatpp::network::Server server(connectionProvider, connectionHandler);
    theServer = &server;

    /* Print info about server port */
    OATPP_LOGi("WEBIF", "Server running on port {}",
               connectionProvider->getProperty("port").toString());

    /* Run server */
    server.run();

    OATPP_LOGi("WEBIF", "Server stopped on port {}",
               connectionProvider->getProperty("port").toString());
}

/**
 *  main
 */
static int theMainLoop()
{
    oatpp::Environment::init();

    run();

    /* Print how much objects were created during app running, and what have left-probably leaked */
    /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
    std::cout << "\nEnvironment:\n";
    std::cout << "objectsCount = " << oatpp::Environment::getObjectsCount()
              << "\n";
    std::cout << "objectsCreated = " << oatpp::Environment::getObjectsCreated()
              << "\n\n";

    oatpp::Environment::destroy();

    return 0;
}


void webAppStart()
{
    runner = new std::thread(theMainLoop);
}

void webAppStop()
{
    if (theServer) {
        theServer->stop();
        theServer = nullptr;

        if (runner) {
        }
        runner = nullptr;
    }
}

}  // namespace webapi
}  // namespace toffy
