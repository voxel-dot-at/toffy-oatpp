
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
    this->fc = in.optUInt("fc", -1);

    {
        std::lock_guard<oatpp::async::Lock> guard(this->shared_lock);
        this->fc = in.optUInt("fc", -1);
        sharedMat = &out;

        for (auto iter = listeners.begin(); iter != listeners.end(); iter++) {
            WebListener* weli = *iter;

            weli->haveWork(in);
        }

        for (auto iter = nexties.begin(); iter != nexties.end(); iter++) {
            (*iter)->haveWork(in);
        }
        nexties.clear();
    }
    newFrameSema.notifyAll();
    shared_cv.notifyAll(); // new frame..

    sharedMat = nullptr;

    return true;
}

class MyCoroutine : public oatpp::async::Coroutine<MyCoroutine>
{
    oatpp::async::LockGuard lockGuard;
    oatpp::async::ConditionVariable* newFrameSema;  ///< defined in webadapter
   public:
    MyCoroutine(oatpp::async::Lock* lock, oatpp::async::ConditionVariable* c)
        : lockGuard(lock), newFrameSema(c)
    {
    }

    Action act() override
    {
        cout << "MyCoroutine::act waitng.." << endl;
        return newFrameSema
            ->wait(lockGuard,
                   [this]() noexcept {
                       cout << "MyCoroutine::act waited!" << endl;
                       return true;
                   })
            .next(finish());
    }

    Action onReady() { return finish(); }
};

oatpp::async::Action WebAdapter::fetchNextFrame(WebListener* weli)
{
    weli->newFrameSema = &newFrameSema;
    nexties.push_back(weli);
    cout << "WebAdapter::fetchNextFrame " << weli->name() << endl;
    //    return weli->waitForNextFrame();

    oatpp::async::Executor executor;

    executor.execute<MyCoroutine>(&lock, &newFrameSema);

    executor.waitTasksFinished();
    executor.stop();
    executor.join();
    cout << "WebAdapter::fetchNextFrame FETCHED " << weli->name() << endl;
}

