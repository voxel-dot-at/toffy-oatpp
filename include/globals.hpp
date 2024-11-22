#pragma once

#include <string>

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
};

/// @brief global application state; to be defined in the main application
extern SystemState theState;
