
#include <string>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/log/trivial.hpp>

#include <toffy/filter_helpers.hpp>

#include "oatpp/async/Executor.hpp"
#include "oatpp/async/Coroutine.hpp"
#include "oatpp/async/ConditionVariable.hpp"

#include "toffy_oatpp/toffy/webAdapter.hpp"

using namespace toffy_oatpp;
using namespace toffy_oatpp::webapi;
using namespace toffy;
using namespace cv;
using namespace std;

static const bool doVisu = false;
namespace toffy_oatpp {
namespace webapi {

toffy::Filter* CreateWebAdapter(void) { return new WebAdapter(); }

}  // namespace webapi
}  // namespace toffy_oatpp

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
    if (!api) {
        return false;
    }
    unsigned int fc = in.optUInt("fc", -1);
    float ledTemp = in.optFloat("lt", -1);
    float mainTemp = in.optFloat("mt", -1);

    {
        std::lock_guard<oatpp::async::Lock> guard(api->lock);
        api->fc = fc;
        api->ledTemp = ledTemp;
        api->mainTemp = mainTemp;
        api->frame = &in;

        if (api->wantDepth && in.hasKey(api->depthSlotName)) {
            matPtr m = in.optMatPtr(api->depthSlotName, 0);

            double ui_minVal = api->depthSettings.ui_minVal;
            double ui_maxVal = api->depthSettings.ui_maxVal;
            double alpha, beta, minVal, maxVal;

            // alpha: scaling factor for contrast adjustment in the image
            // beta: offset for brightness adjustment
            if (ui_maxVal >= 0 && ui_minVal >= 0 && ui_maxVal > ui_minVal) {
                alpha = 255.0 / (ui_maxVal - ui_minVal);
                beta = -ui_minVal * 255.0 / (ui_maxVal - ui_minVal);
            } else {
                cv::minMaxLoc(*m, &minVal, &maxVal);

                api->depthSettings.maxVal = maxVal;
                api->depthSettings.minVal = minVal;

                alpha = 255.0 / (maxVal - minVal);
                beta = -minVal * 255.0 / (maxVal - minVal);
            }

            cv::Mat scaledDepth;
            m->convertTo(scaledDepth, CV_8U, alpha, beta);

            cv::Mat coloredDepth;
            cv::applyColorMap(scaledDepth, coloredDepth, cv::COLORMAP_JET);

            auto jpeg = compressMat2Jpeg(coloredDepth);
            api->depthJpeg = jpeg;
            api->wantDepth = false;
        }

        if (api->wantAmpl && in.hasKey(api->amplSlotName)) {
            matPtr m = in.optMatPtr(api->amplSlotName, 0);

            double ui_minVal = api->amplSettings.ui_minVal;
            double ui_maxVal = api->amplSettings.ui_maxVal;
            double alpha, beta, minVal, maxVal;

            // alpha: scaling factor for contrast adjustment in the image
            // beta: offset for brightness adjustment
            if (ui_maxVal >= 0 && ui_minVal >= 0 && ui_maxVal > ui_minVal) {
                alpha = 255.0 / (ui_maxVal - ui_minVal);
                beta = -ui_minVal * 255.0 / (ui_maxVal - ui_minVal);
            } else {
                cv::minMaxLoc(*m, &minVal, &maxVal);

                api->amplSettings.maxVal = maxVal;
                api->amplSettings.minVal = minVal;

                alpha = 255.0 / (maxVal - minVal);
                beta = -minVal * 255.0 / (maxVal - minVal);
            }

            cv::Mat scaledDepth;
            m->convertTo(scaledDepth, CV_8U, alpha, beta);

            auto jpeg = compressMat2Jpeg(scaledDepth);
            api->amplJpeg = jpeg;
            api->wantAmpl = false;
        }
    }
    api->cv.notifyAll();

    return true;
}

std::shared_ptr<std::string> WebAdapter::compressMat2Jpeg(const cv::Mat& img)
{
    // imgencode params:
    std::vector<int> params;
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(90);  // 0..100 ; 100=high quality

    std::vector<uchar> outbuf(img.rows * img.cols * 3);

    cv::imencode(".jpg", img, outbuf, params);
    size_t len = outbuf.size();
    char* s = (char*)&outbuf[0];

    // constructor from buffer: Copies the first len characters from the array
    // of characters pointed by s
    // std::string* str = new string(s, len);
    auto str = make_shared<string>(s, len);
    return str;
}

bool WebAdapter::filterOld(const Frame& in, Frame& out)
{
    // bool worked = this->lock.try_lock();
    // if (!worked) {
    //     cout << "WebAdapter::filter() FAILED TO LOCK " << worked << " " << listeners.size() << " single " << singleShots.size() << endl;
    // }

    this->fc = in.optUInt("fc", -1);

    cout << "WebAdapter::filter() listeners..? " << listeners.size()
         << " single " << singleShots.size() << " rc " << resource.counter
         << endl;
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

        for (auto iter = singleShots.begin(); iter != singleShots.end();
             iter++) {
            (*iter)->haveWork(in);
        }
        singleShots.clear();
    }
    cout << "WebAdapter::filter() GO"
         << " rc " << resource.counter << endl;
    // this->lock.unlock();
    theCv().notifyAll();

    resource.counter = 0;
    cout << "WebAdapter::filter() DONE"
         << " rc " << resource.counter << endl;
    return true;
}
