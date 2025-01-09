#pragma once

#include <string>

#include "toffy_oatpp/shared/syncApi.hpp"
#include "toffy_oatpp/filters/webAdapter.hpp"

namespace toffy {

class Player;

namespace capturers {

class Bta;

}
}  // namespace toffy

/**
 * @brief holds main program state...
 *
 */
class SystemState
{
   public:
    SystemState() {}
    SystemState(const SystemState &) = delete;
    SystemState &operator=(const SystemState &) = delete;

    /// @brief webRoot is used by the staticContentsController as a path prefix to serve local files.
    std::string webRoot = "./web/";

    bool enableSwaggerUi = true;

    toffy::Player *player = nullptr;
    toffy::capturers::Bta *bta = nullptr;
    toffy::webapi::WebAdapter *webAdap = nullptr;

    /// @brief the synchronisation API for syncing OpenCV threads against Oat++
    SyncApi api;
};

/** set up the global state object, set an existing toffy player object 
 * and register a callback filter in the filterbank for syncing with the frontend.
 */
extern void setupGlobalState(SystemState &state, bool withSwagger, toffy::Player *player);

/// @brief global application state; to be defined in the main application
// extern SystemState theState;

extern SystemState& getGlobalState();
