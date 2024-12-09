#pragma once

#include "oatpp/async/ConditionVariable.hpp"
#include "oatpp/async/Lock.hpp"

#include <toffy/frame.hpp>

/// the global i to sync backend process and frontend api:

class SyncApi
{
   public:
    oatpp::async::Lock lock;
    oatpp::async::ConditionVariable cv;

    bool keepRunning = true;

    /// the frame counter set by the thread
    int fc = -1;

    // anything else to be shared goes below here...
    int exposure = 123;

    const toffy::Frame* frame = nullptr;
    std::string* zJpeg = nullptr;
};