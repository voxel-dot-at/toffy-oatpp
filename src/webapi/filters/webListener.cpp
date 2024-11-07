
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



oatpp::async::Action WebListener::waitForWork()
{
    cout << "WAIT " << name() << endl;

    // std::unique_lock lk(m);
    isWaiting = true;

    return newFrameSema
        ->wait(m_lockGuard,
               [this]() noexcept {
                   isWaiting = false;
                   cout << "WAITed " << name() << " " << valid << endl;
                   return valid;
               })
        .next(finish());
}

oatpp::async::Action WebListener::waitForNextFrame()
{
    cout << "WebListener::waitForNextFrame " << name() << endl;
    isWaiting = true;

    return newFrameSema
        ->wait(m_lockGuard,
               [this]() noexcept {
                   isWaiting = false;
                   cout << "WAITed " << name() << " " << valid << endl;
                   return valid;
               })
        .next(finish());
}

void WebListener::haveWork(const Frame& in)
{
    valid = false;
    cout << "haveWork...? " << name() << " " << isWaiting << endl;
    if (isWaiting) {
        {
            // std::lock_guard lk(m);
            valid = process(in);
        }
        // manual unlocking is done before notifying, to avoid waking up
        // the waiting thread only to block again (see notify_one for details)
        // lk.unlock();

        // workToDo.notify_one();
    }
}

bool FrameInfoListener::process(const Frame& in)
{
    slots.clear();
    in.info(slots);

    // ts = in.optUInt("ts", -1);
    fc = in.optUInt("fc", 0);
    ledTemp = in.optFloat("lt", -1);
    mainTemp = in.optFloat("mt", -1);
    generalTemp = in.optFloat("gt", -1);

    cout << "FrameInfoListener::process()" << endl;
    auto it = slots.begin();
    while (it != slots.end()) {
        auto si = *it;
        cout << "   " << si.key << " " << si.dt << " |" << si.description
             << endl;
        it++;
    }

    return true;
}

bool ImgListener::process(const Frame& in)
{
    matPtr img = in.optMatPtr(name(), 0);
    if (!img) {
        BOOST_LOG_TRIVIAL(warning)
            << "ImgListener::process() could not get " << name();

        return false;
    }

    // imgencode params:
    std::vector<int> params;
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(90);  // 0..100 ; 100=high quality

    std::vector<uchar> outbuf(img->rows * img->cols * 3);

    cv::imencode(".jpg", *img, outbuf, params);
    size_t len = outbuf.size();
    char* s = (char*)&outbuf[0];

    // constructor from buffer: Copies the first len characters from the array
    // of characters pointed by s
    // std::string* str = new string(s, len);
    strVal.reset(new string(s, len));
    return true;
}

bool NumberListener::process(const Frame& in)
{
    if (in.hasKey(name())) {
        numVal = in.getInt(name());
    } else {
        BOOST_LOG_TRIVIAL(warning)
            << "NumberListener::process() could not find " << name();
        return false;
    }
    return true;
}
