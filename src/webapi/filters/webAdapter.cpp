
#include <string>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/log/trivial.hpp>

#include <toffy/filter_helpers.hpp>

#include "webapi/filters/webAdapter.hpp"

#include "oatpp/async/Executor.hpp"
#include "oatpp/async/Coroutine.hpp"
#include "oatpp/async/ConditionVariable.hpp"

using namespace toffy;
using namespace toffy::webapi;
using namespace cv;
using namespace std;

static const bool doVisu = false;

toffy::Filter* toffy::webapi::CreateWebAdapter(void)
{
    return new WebAdapter();
}

std::size_t WebAdapter::_filter_counter = 1;
const std::string WebAdapter::id_name = "webAdapter";

WebAdapter::WebAdapter() : Filter(WebAdapter::id_name, _filter_counter)
{
    _filter_counter++;
    // bool worked = this->lock.try_lock();
    // cout << "WebAdapter::WebAdapter() locked? " << worked << endl;

}

// virtual int loadConfig(const boost::property_tree::ptree& pt);
boost::property_tree::ptree WebAdapter::getConfig() const
{
    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " " << id();
    boost::property_tree::ptree pt;

    pt = Filter::getConfig();
    // todo: add options
    return pt;
}

void WebAdapter::updateConfig(const boost::property_tree::ptree& pt)
{
    using namespace boost::property_tree;

    Filter::updateConfig(pt);
}

bool WebAdapter::filter(const Frame& in, Frame& out)
{
    // bool worked = this->lock.try_lock();
    // if (!worked) {
    //     cout << "WebAdapter::filter() FAILED TO LOCK " << worked << " " << listeners.size() << " single " << singleShots.size() << endl;
    // }

    this->fc = in.optUInt("fc", -1);

    cout << "WebAdapter::filter() listeners..? " << listeners.size() << " single " << singleShots.size() << " rc " << resource.counter <<  endl;
    {
        std::lock_guard<oatpp::async::Lock> guard(lock);
        // std::lock_guard<oatpp::async::Lock> guard(this->lock);
        this->fc = in.optUInt("fc", -1);
        this->resource.counter++;
        this->resource.frame = &out;

        for (auto iter = listeners.begin(); iter != listeners.end(); iter++) {
            WebListener* weli = *iter;

            weli->haveWork(in);
        }

        for (auto iter = singleShots.begin(); iter != singleShots.end(); iter++) {
            (*iter)->haveWork(in);
        }
        singleShots.clear();
    }
    cout << "WebAdapter::filter() GO" << " rc " << resource.counter << endl;
    // this->lock.unlock();
    theCv().notifyAll(); 

    resource.counter = 0;
    cout << "WebAdapter::filter() DONE" << " rc " << resource.counter << endl;
    return true;
}

