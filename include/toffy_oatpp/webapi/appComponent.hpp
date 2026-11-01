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

#pragma once

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"

#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/json/ObjectMapper.hpp"

#include "oatpp/macro/component.hpp"

#if TOFFYOATPP_WITH_SWAGGER
#include "toffy_oatpp/webapi/swaggerComponent.hpp"
#endif

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent
{
   public:
    /**
   *  Swagger component — debug/diagnostic only, and absent from a build
   *  configured with -DTOFFYOATPP_WITH_SWAGGER=OFF (which is also what drops the
   *  oatpp-swagger dependency; the flag arrives on the target's interface, so it
   *  describes the library being linked rather than the tree being compiled).
   */
#if TOFFYOATPP_WITH_SWAGGER
    SwaggerComponent swaggerComponent;
#endif

    /**
   *  Create Router component
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                           httpRouter)
    ([] { return oatpp::web::server::HttpRouter::createShared(); }());

    /*
     * The connection provider and the connection handler used to be components
     * here, and are now built by `webApiStart` (see src/webapi/webApi.cpp).
     *
     * `OATPP_CREATE_COMPONENT(type, name)([] { ... }())` — the trailing `()` —
     * does not register a factory, it registers the *result*: everything below
     * is constructed the moment an AppComponent is. `webApi.cpp` holds one at
     * file scope, so "the moment an AppComponent is" was "when the library was
     * loaded", before main. Measured against a `int main(){ return 0; }` linked
     * only against libtoffy_oatpp.so:
     *
     *   * 62 threads at the first line of main;
     *   * `00000000:1F40` state 0A in /proc/net/tcp — listening on 0.0.0.0:8000,
     *     in a program that never asked for a web api and has no way to decline
     *     one;
     *   * SIGSEGV (core dumped) at exit, in `oatpp::async::Processor::popTasks`,
     *     the Processor destroyed by static teardown with its workers running.
     *
     * The threads are the AsyncHttpConnectionHandler's oatpp `Processor`; the
     * socket is the ConnectionProvider's. Both are only needed to *run* a
     * server, so both are now created there.
     *
     * What stays here is deliberately not nothing: `ContentMappers` must be
     * registered before any `oatpp::web::server::api::ApiController` is
     * constructed — its default endpoints mapper asks the Environment for it.
     * Applications build their controllers before calling `webApiStart`
     * (`boxes` does: make_shared, registerController, setupGlobalState, start),
     * so making the whole component set lazy is not an option; measured, it
     * dies with
     *
     *   terminate: [oatpp::Environment::getComponent()]: Component of given
     *   type doesn't exist: type='...ContentMappers'
     *
     * That is the line this file has to stay on: cheap and needed-by-everyone
     * here, sockets and threads at start.
     */

    /**
   *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                           apiContentMappers)
    ([] {
        auto json = std::make_shared<oatpp::json::ObjectMapper>();
        json->serializerConfig().json.useBeautifier = true;

        auto mappers = std::make_shared<oatpp::web::mime::ContentMappers>();
        mappers->putMapper(json);

        return mappers;
    }());
};

