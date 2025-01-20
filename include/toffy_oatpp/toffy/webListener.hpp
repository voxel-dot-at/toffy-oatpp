#pragma once

/**
 * @brief find a colored box via hsv analysis
 * 
 */
#pragma once

#include <string>

#include <condition_variable>
#include <mutex>
#include <vector>
#include <thread>

#include "oatpp/async/Executor.hpp"
#include "oatpp/async/Coroutine.hpp"
#include "oatpp/async/ConditionVariable.hpp"

#include "toffy_oatpp/toffy/webAdapter.hpp"

namespace toffy_oatpp {
// namespace toffy {

// from WebAdapter, we have
// class WebAdapterListener;

class FrameInfoListener : public toffy_oatpp::WebAdapterListener
{
   public:
    FrameInfoListener(const std::string& name = "frame")
        : WebAdapterListener(name)
    {}

    // std::vector<std::string> keys;
    std::vector<toffy::Frame::SlotInfo> slots;
    int fc;
    float ledTemp, mainTemp;

   protected:
    virtual bool process(const toffy::Frame& in);
};

class ImgListener : public WebAdapterListener
{
   public:
    ImgListener(const std::string& name) : WebAdapterListener(name) {}

    boost::shared_ptr<std::string> strVal;

   protected:
    virtual bool process(const toffy::Frame& in);
};

class NumberListener : public WebAdapterListener
{
   public:
    NumberListener(const std::string& name) : WebAdapterListener(name) {}

    long numVal;

   protected:
    virtual bool process(const toffy::Frame& in);
};

// }  // namespace webapi
}  // namespace toffy_oatpp
