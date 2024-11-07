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

class WebAdapter : public Filter
{
    static const std::string id_name;  ///< Filter identifier
   public:
    WebAdapter();

    virtual ~WebAdapter() {}

    unsigned int fc = -1;

    oatpp::async::ConditionVariable newFrameSema;
    // oatpp::async::LockGuard m_lockGuard;
    oatpp::async::Lock lock;

    std::vector<WebListener*> listeners;

    // virtual int loadConfig(const boost::property_tree::ptree& pt);
    virtual boost::property_tree::ptree getConfig() const;

    virtual void updateConfig(const boost::property_tree::ptree& pt);

    virtual bool filter(const Frame& in, Frame& out);

    // registers from an extrnal thread, will be released via newFrameSema:
    oatpp::async::Action fetchNextFrame(WebListener* weli);

    std::vector<WebListener*> nexties;


    // shared resource: new frame ...

    Frame* sharedMat;
    // std::shared_ptr<Resource> resource;
    oatpp::async::Lock shared_lock;
    oatpp::async::ConditionVariable shared_cv;

   private:
    static size_t _filter_counter;
};

extern toffy::Filter* CreateWebAdapter(void);

}  // namespace webapi
}  // namespace toffy

#endif