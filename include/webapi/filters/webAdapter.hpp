#ifndef WEB_ADAPTER_
#define WEB_ADAPTER_

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
// #include "oatpp/core/data/resource/Resource.hpp"
#include "oatpp/async/ConditionVariable.hpp"

#include "webapi/filters/webListener.hpp"

namespace toffy {
namespace webapi {

struct Resource
{
    v_int64 counter;
    Frame* frame;
};

class WebAdapter : public Filter
{
    static const std::string id_name;  ///< Filter identifier

    oatpp::async::ConditionVariable newFrameSema;
    oatpp::async::Lock lock;

   public:
    WebAdapter();

    virtual ~WebAdapter() {}

    unsigned int fc = -1;

    oatpp::async::Lock& theLock() { return lock; }
    oatpp::async::ConditionVariable& theCv() { return newFrameSema; }
    volatile Resource resource;

    std::vector<WebListener*> listeners;

    // virtual int loadConfig(const boost::property_tree::ptree& pt);
    virtual boost::property_tree::ptree getConfig() const;

    virtual void updateConfig(const boost::property_tree::ptree& pt);

    virtual bool filter(const Frame& in, Frame& out);

    // registers from an extrnal thread, will be released via newFrameSema:
    // oatpp::async::Action fetchNextFrame(WebListener* weli);

    void singleShot(WebListener* wl) { nexties.push_back(wl); }

   private:
    static size_t _filter_counter;
    std::vector<WebListener*> nexties;
};

extern toffy::Filter* CreateWebAdapter(void);

}  // namespace webapi
}  // namespace toffy

#endif