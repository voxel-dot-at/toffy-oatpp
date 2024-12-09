
#include <iostream>
#include <thread>

#include "oatpp/network/Server.hpp"

#include "oatpp-swagger/Controller.hpp"
#include "oatpp-swagger/AsyncController.hpp"

#include "webapi/controller/staticContentsController.hpp"
#include "webapi/controller/btaAdapterController.hpp"
#include "webapi/controller/webAdapterController.hpp"
#include "webapi/controller/frameInfoController.hpp"

#include "filters/webAdapter.hpp"
#include "webapi/swaggerComponent.hpp"
#include "webapi/appComponent.hpp"

#include "globals.hpp"

static oatpp::network::Server* theServer = nullptr;
static std::thread* runner = nullptr;
static toffy::webapi::WebAdapter* webAdap = nullptr;
static SystemState* systemState = nullptr;

using namespace toffy::webapi;

namespace toffy {
namespace webapi {

/** helper class to enforce calling init() before initializing the AppComponent(!) */
class Init
{
   public:
    Init() { oatpp::Environment::init(); }
};

class WebApi
{
   public:
    Init init;  // n.b. keep these two in order!
    AppComponent components;

    std::vector<std::shared_ptr<oatpp::web::server::api::ApiController>>
        controllers;

    WebApi() {}

    void run()
    {
        /* Get router component */
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                        router);

        oatpp::web::server::api::Endpoints docEndpoints;

        /**
         * REGISTER CONTROLLERS
         */

        for (size_t i = 0; i < controllers.size(); i++) {
            auto ctrl = controllers[i];
            router->addController(ctrl);
            docEndpoints.append(ctrl->getEndpoints());
        }

        if (theState.enableSwaggerUi) {
            router->addController(
                oatpp::swagger::AsyncController::createShared(docEndpoints));
        }

        // static contents:
        router->addController(std::make_shared<StaticContentsController>());

        /**
         * /REGISTER CONTROLLERS
         */

        /* Get connection handler component */
        OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                        connectionHandler);

        /* Get connection provider component */
        OATPP_COMPONENT(
            std::shared_ptr<oatpp::network::ServerConnectionProvider>,
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
};

WebApi api;

void webAppInit() {}

void registerController(
    std::shared_ptr<oatpp::web::server::api::ApiController> controller)
{
    api.controllers.push_back(controller);
}

static void addInternalControllers()
{
    //    registerController(std::make_shared<WebAdapterController>());
}

/**
 *  main
 */
static int theMainLoop()
{
    addInternalControllers();

    api.run();

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

void webAppStart(toffy::webapi::WebAdapter* webAdapter, SystemState* state)
{
    webAdap = webAdapter;
    systemState = state;
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
