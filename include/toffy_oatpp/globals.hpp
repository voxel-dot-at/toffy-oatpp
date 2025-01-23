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

#include <string>

#include "toffy_oatpp/shared/syncApi.hpp"
#include "toffy_oatpp/toffy/webAdapter.hpp"

namespace toffy {

class Player;

namespace capturers {

class Bta;

}
}  // namespace toffy

namespace toffy_oatpp {
/**
 * @brief holds main program state...
 *
 */
class WebApiState
{
   public:
    WebApiState() {}
    WebApiState(const WebApiState &) = delete;
    WebApiState &operator=(const WebApiState &) = delete;

    /// @brief webRoot is used by the staticContentsController as a path prefix to serve local files.
    std::string webRoot = "./web/";

    bool enableSwaggerUi = true;

    toffy::Player *player = nullptr;
    toffy::capturers::Bta *bta = nullptr;

    toffy_oatpp::webapi::WebAdapter *webAdap = nullptr;

    /// @brief the synchronisation API for syncing OpenCV threads against Oat++
    SyncApi api;
};

/** set up the global state object, set an existing toffy player object 
 * and register a callback filter in the filterbank for syncing with the frontend.
 */
extern void setupGlobalState(WebApiState &state, bool withSwagger,
                             toffy::Player *player);

/// @brief global application state; to be defined in the main application
// extern WebApiState theState;

extern WebApiState &getGlobalState();

}  // namespace toffy_oatpp