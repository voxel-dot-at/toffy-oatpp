#pragma once
#include <vector>

#include "oatpp/async/ConditionVariable.hpp"
#include "oatpp/async/Lock.hpp"

#include <toffy/frame.hpp>

/// the global api to sync backend process and frontend api:

typedef struct {

    double minVal = -1;
    double maxVal = -1;
    double ui_minVal = -1;
    double ui_maxVal = -1;
} MinMaxSettings;

class SyncApi
{
   public:
    oatpp::async::Lock lock;
    oatpp::async::ConditionVariable cv;

    bool keepRunning = true;

    /// the frame counter set by the thread
    int fc = -1;
    int ledTemp = -1;
    int mainTemp = -1;

    // anything else to be shared goes below here...
    int exposure = 123;

    // a pointer to the current frame for querying
    const toffy::Frame* frame = nullptr;

    // image compression - todo: refactor the following to something more flexible.

    bool wantDepth = false;
    bool wantAmpl = false;
    std::string depthSlotName = "depth";
    std::string amplSlotName = "ampl";
    
    std::shared_ptr<std::string> depthJpeg = nullptr;
    std::shared_ptr<std::string> amplJpeg = nullptr;

    MinMaxSettings depthSettings;
    MinMaxSettings amplSettings;
};