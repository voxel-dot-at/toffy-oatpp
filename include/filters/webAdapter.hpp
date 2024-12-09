#pragma once
/**
 * @brief find a colored box via hsv analysis
 * 
 */


#include <condition_variable>
#include <mutex>
#include <vector>
#include <string>
#include <thread>

#include "toffy/filter.hpp"

#include "oatpp/async/Coroutine.hpp"
#include "oatpp/async/ConditionVariable.hpp"

#include "shared/syncApi.hpp"

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

    oatpp::async::ConditionVariable cv;
    oatpp::async::Lock lock;

    SyncApi* api = nullptr;

   public:
    WebAdapter();

    virtual ~WebAdapter() {}

    void setApi(SyncApi* api) { this->api = api;}

    unsigned int fc = -1;

    oatpp::async::Lock& theLock() { return lock; }
    oatpp::async::ConditionVariable& theCv() { return cv; }
    volatile Resource resource;

    std::vector<WebListener*> listeners;

    // virtual int loadConfig(const boost::property_tree::ptree& pt);
    virtual boost::property_tree::ptree getConfig() const;

    virtual void updateConfig(const boost::property_tree::ptree& pt);

    virtual bool filter(const Frame& in, Frame& out);

    // old stuff, goes away
    virtual bool filterOld(const Frame& in, Frame& out);

    // registers from an extrnal thread, will be released via newFrameSema:
    // oatpp::async::Action fetchNextFrame(WebListener* weli);

    void singleShot(WebListener* wl) { singleShots.push_back(wl); }

   private:
    static size_t _filter_counter;
    std::vector<WebListener*> singleShots;

    std::string* compressMat2Jpeg(const cv::Mat& mat);
};

extern toffy::Filter* CreateWebAdapter(void);

}  // namespace webapi
}  // namespace toffy
