#pragma once

#include "globals.hpp"

namespace toffy_oatpp {


/** set up the global state object, set an existing toffy player object 
 * and register a callback filter in the filterbank for syncing with the frontend.
 */
extern void setupGlobalState(WebApiState &state, bool withSwagger, toffy::Player *player);

/// @brief global application state; to be defined in the main application
// extern WebApiState theState;

extern WebApiState& getGlobalState();




}
