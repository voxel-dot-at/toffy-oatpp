#ifndef BTA_ADAPTER_
#define BTA_ADAPTER_

/**
 * @brief find a colored box via hsv analysis
 * 
 */
#pragma once

#include <string>

#include <condition_variable>
#include <mutex>
#include <vector>

#include "toffy/filter.hpp"

namespace toffy {
namespace oatpp {


/** data listener super class; implements thread sync artefacts */
class WebListener {
    std::string n;
    public:
    WebListener(const std::string& name) : n(name) {} 

    const std::string& name() const { return n; }

    // wait on semaphore
    void waitForWork();

    // notify semaphore
    void haveWork(const Frame& in);

    bool valid;
protected:
    std::mutex m;
    std::condition_variable workToDo;

    // isWaiting is set to true when a web requeset is pending; do work only if this is true.
    bool isWaiting = false;
    // do something with the frame data
    virtual bool process(const Frame& in) = 0;
};


class FrameInfoListener: public WebListener {
    public:
    FrameInfoListener(const std::string& name): WebListener(name) {}

    // std::vector<std::string> keys; 
    std::vector<Frame::SlotInfo> slots; 

protected:
    virtual bool process(const Frame& in);
};

class ImgListener: public WebListener {
    public:
    ImgListener(const std::string& name): WebListener(name) {}

    boost::shared_ptr<std::string> strVal; 

protected:
    virtual bool process(const Frame& in);
};

class NumberListener: public WebListener {
    public:
    NumberListener(const std::string& name): WebListener(name) {}

    long numVal;

protected:
    virtual bool process(const Frame& in);
};

class BtaAdapter : public Filter
{
    static const std::string id_name;  ///< Filter identifier
   public:
    WebAdapter();

    virtual ~WebAdapter() {}

    unsigned int fc = -1;

    std::vector<WebListener*> listeners;

    // virtual int loadConfig(const boost::property_tree::ptree& pt);
    virtual boost::property_tree::ptree getConfig() const;

    virtual void updateConfig(const boost::property_tree::ptree& pt);

    virtual bool filter(const Frame& in, Frame& out);

   private:
    static size_t _filter_counter;
};

extern toffy::Filter* CreateWebAdapter(void);

}  // namespace oatpp
}  // namespace toffy

#endif