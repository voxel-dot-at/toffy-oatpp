/*
   Copyright 2012-2025 Simon Vogl <svogl@voxel.at> VoXel Interaction Design - www.voxel.at

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <iostream>
#include <thread>

#include "oatpp/network/Server.hpp"

#if TOFFYOATPP_WITH_SWAGGER
#include "oatpp-swagger/Controller.hpp"
#include "oatpp-swagger/AsyncController.hpp"
#endif

#include "toffy_oatpp/webapi/controller/staticContentsController.hpp"
#if TOFFYOATPP_WITH_BTA
#include "toffy_oatpp/webapi/controller/btaAdapterController.hpp"
#endif
#include "toffy_oatpp/webapi/controller/frameInfoController.hpp"
#include "toffy_oatpp/webapi/controller/staticContentsController.hpp"

#include "toffy_oatpp/toffy/webAdapter.hpp"

#if TOFFYOATPP_WITH_SWAGGER
#include "toffy_oatpp/webapi/swaggerComponent.hpp"
#endif
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

        // The swagger UI is a debugging surface, and it is now also a build
        // decision: with TOFFYOATPP_WITH_SWAGGER=0 neither the component nor
        // oatpp-swagger itself is in this binary, and the runtime flag below has
        // nothing to switch. A production build that never serves /swagger/ui
        // therefore does not carry a pinned-until-1.5 dependency to do it, and a
        // caller that asks for the UI at runtime on such a build gets the same
        // answer it would get from a router with no such route: a 404.
#if TOFFYOATPP_WITH_SWAGGER
        if (systemState->enableSwaggerUi) {
            router->addController(
                oatpp::swagger::AsyncController::createShared(docEndpoints));
        }
#endif

        // static contents:
        router->addController(std::make_shared<StaticContentsController>());

        /**
         * /REGISTER CONTROLLERS
         */

        /*
         * The connection handler and its provider are built here, on the thread
         * that is about to serve, and not as eagerly-constructed components.
         * They used to be components in AppComponent, which — with the
         * file-scope `static WebApi api;` below — meant the socket was bound and
         * the async Processor's worker threads were running before main(), in
         * every program that merely links this library. Measured: 62 threads and
         * `0.0.0.0:8000` in LISTEN at the first line of a `return 0` main, and a
         * SIGSEGV at exit in Processor::popTasks. The full measurement and the
         * reason ContentMappers cannot move here with these two is in
         * include/toffy_oatpp/webapi/appComponent.hpp.
         *
         * The port is still the one the component used to pass. Making it a
         * WebApiState field is a behaviour change waiting for a config key, not
         * a side effect of this one.
         */
        auto connectionProvider =
            oatpp::network::tcp::server::ConnectionProvider::createShared(
                {"0.0.0.0", 8000, oatpp::network::Address::IP_4});

        auto connectionHandler =
            oatpp::web::server::AsyncHttpConnectionHandler::createShared(router);

        /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
        oatpp::network::Server server(connectionProvider, connectionHandler);
        theServer = &server;

        /* Print info about server port */
        OATPP_LOGi("WEBAPI", "Server running on port {}",
                   connectionProvider->getProperty("port").toString());

        /* Run server */
        server.run();

        OATPP_LOGi("WEBAPI", "Server stopped on port {}",
                   connectionProvider->getProperty("port").toString());
    }
};

static WebApi api;

void registerController(
    std::shared_ptr<oatpp::web::server::api::ApiController> controller)
{
    api.controllers.push_back(controller);
}

static void addInternalControllers()
{
#if TOFFYOATPP_WITH_BTA
    if (systemState->bta) {
        OATPP_LOGd("WEBAPI", "registering BTA controller for {}", systemState->bta->name());

        auto bac = std::make_shared<BtaAdapterController>();
        bac->registerBta(systemState->bta);
        registerController(bac);
    }
#else
    // Built against a toffy without the bta SDK (P1-13): the /api/bta* endpoints
    // are not in this binary at all — not 404, not 503, not in the swagger
    // listing. That matches the runtime rule above (no bta filter, no controller)
    // rather than inventing a new one; a 503 would advertise a camera this build
    // can never talk to. The CMake configure line states which case this is.
    OATPP_LOGw("WEBAPI", "BTA support not compiled in — /api/bta* endpoints are not "
                         "available (libtoffy was built without the bta SDK)");
#endif
    //    registerController(std::make_shared<WebAdapterController>());
}

/**
 *  main
 */
static int webApiRunner()
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

void webApiStart(toffy_oatpp::WebApiState& state)
{
    cout << "TOFFY_OATPP::WEBAPPSTART" << endl;
    systemState = &state;
    runner = new std::thread(webApiRunner);
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
}  // namespace toffy_oatpp
