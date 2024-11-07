#pragma once


/**
 * @brief holds main program state...
 * 
 */
class SystemState
{
   public:
    toffy::Player* player = nullptr;
    toffy::capturers::Bta* bta = nullptr;

    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;
};

extern SystemState theState;
