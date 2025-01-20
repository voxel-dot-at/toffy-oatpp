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

#include "toffy/frame.hpp"
#include "toffy/filter.hpp"

// #include "oatpp/async/Coroutine.hpp"
// #include "oatpp/async/ConditionVariable.hpp"

#include "toffy_oatpp/shared/syncApi.hpp"

namespace toffy_oatpp {

class WebAdapterListener
{
    std::string n;

   public:
    bool runMe;

    WebAdapterListener(const std::string& name) : n(name) {}

    const std::string& name() const { return n; }

    // do something with the frame data
    virtual bool process(const ::toffy::Frame& in, SyncApi* api) = 0;
};

namespace webapi {

class WebAdapter : public toffy::Filter
{
    static const std::string id_name;  ///< Filter identifier

    SyncApi* api = nullptr;

   public:
    WebAdapter();

    virtual ~WebAdapter() {}

    void setApi(SyncApi* api) { this->api = api; }

    unsigned int fc = -1;

    std::vector<WebAdapterListener*> listeners;

    // virtual int loadConfig(const boost::property_tree::ptree& pt);
    virtual boost::property_tree::ptree getConfig() const;

    virtual void updateConfig(const boost::property_tree::ptree& pt);

    virtual bool filter(const toffy::Frame& in, toffy::Frame& out);

   private:
    static size_t _filter_counter;

    std::shared_ptr<std::string> compressMat2Jpeg(const cv::Mat& mat);
};

extern toffy::Filter* CreateWebAdapter(void);

}  // namespace webapi
}  // namespace toffy_oatpp
