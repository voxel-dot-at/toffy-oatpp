#pragma once

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

    bool enableSwaggerUi = true;

    toffy::Player *player = nullptr;
    toffy::capturers::Bta *bta = nullptr;

};

extern SystemState theState;
