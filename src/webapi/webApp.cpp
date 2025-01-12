
#include <iostream>
#include <thread>

#include "oatpp/network/Server.hpp"

#include "oatpp-swagger/Controller.hpp"
#include "oatpp-swagger/AsyncController.hpp"

#include "toffy_oatpp/webapi/controller/staticContentsController.hpp"
#include "toffy_oatpp/webapi/controller/btaAdapterController.hpp"
#include "toffy_oatpp/webapi/controller/frameInfoController.hpp"

#include "toffy_oatpp/toffy/webAdapter.hpp"

#include "toffy_oatpp/webapi/swaggerComponent.hpp"
#include "toffy_oatpp/webapi/appComponent.hpp"

#include "toffy_oatpp/globals.hpp"

static oatpp::network::Server* theServer = nullptr;
static std::thread* runner = nullptr;
static toffy_oatpp::WebApiState* systemState = nullptr;

using namespace std;

namespace toffy_oatpp {
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

        if (systemState->enableSwaggerUi) {
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

extern void webApiStart(toffy_oatpp::WebApiState& state)
{
    cout << "TOFFY_OATPP::WEBAPPSTART" << endl;
    systemState = &state;
    runner = new std::thread(theMainLoop);

}

void webApiStop()
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
