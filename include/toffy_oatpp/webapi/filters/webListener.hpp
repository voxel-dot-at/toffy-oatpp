#ifndef WEB_LISTENER_HPP
#define WEB_LISTENER_HPP

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

#include "toffy/filter.hpp"

#include "oatpp/async/Executor.hpp"
#include "oatpp/async/Coroutine.hpp"
#include "oatpp/async/ConditionVariable.hpp"

namespace toffy {
namespace webapi {

/** data listener super class; implements thread sync artefacts */
class WebListener : public oatpp::async::Coroutine<WebListener>
{
    std::string n;

   public:
    WebListener(const std::string& name) : n(name) {}

    Action act() override
    {
        return newFrameSema
            ->wait(m_lockGuard, [this]() noexcept { return valid; })
            .next(finish());
    }

    // Action onReady()
    // {
    //     return finish();
    // }

    const std::string& name() const { return n; }

    std::vector<std::thread> threads;

    // wait on semaphore
    Action waitForWork();

    // wait on semaphore
    Action waitForNextFrame();

    // notify semaphore
    void haveWork(const Frame& in);

    bool valid;

    oatpp::async::LockGuard m_lockGuard;
    oatpp::async::ConditionVariable* newFrameSema;  ///< defined in webadapter

   protected:
    std::mutex m;
    // std::condition_variable workToDo;

    // oatpp::async::LockGuard m_lockGuard;
    oatpp::async::ConditionVariable workTodo;

    // std::shared_ptr<Resource> m_resource;
    // oatpp::async::LockGuard m_lockGuard;
    // oatpp::async::ConditionVariable m_cv;

    // isWaiting is set to true when a web requeset is pending; do work only if this is true.
    bool isWaiting = false;
    // do something with the frame data
    virtual bool process(const Frame& in) = 0;
};

class FrameInfoListener : public WebListener
{
   public:
    FrameInfoListener(const std::string& name = "frame") : WebListener(name) { isWaiting=true; }

    // std::vector<std::string> keys;
    std::vector<Frame::SlotInfo> slots;
    int fc;
    float ledTemp, mainTemp;

   protected:
    virtual bool process(const Frame& in);
};

class ImgListener : public WebListener
{
   public:
    ImgListener(const std::string& name) : WebListener(name) {}

    boost::shared_ptr<std::string> strVal;

   protected:
    virtual bool process(const Frame& in);
};

class NumberListener : public WebListener
{
   public:
    NumberListener(const std::string& name) : WebListener(name) {}

    long numVal;

   protected:
    virtual bool process(const Frame& in);
};

}  // namespace webapi
}  // namespace toffy

#endif