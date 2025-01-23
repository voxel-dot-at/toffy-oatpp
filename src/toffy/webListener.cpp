/*
   Copyright 2012-2025 Simon Vogl <svogl@voxel.at> VoXel Interaction Design - www.voxel.at

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <string>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/log/trivial.hpp>

#include <toffy/filter_helpers.hpp>

#include "oatpp/async/Executor.hpp"
#include "oatpp/async/Coroutine.hpp"
#include "oatpp/async/ConditionVariable.hpp"

#include "toffy_oatpp/toffy/webListener.hpp"

using namespace toffy_oatpp;
using namespace toffy_oatpp::webapi;
using namespace toffy;
using namespace cv;
using namespace std;

static const bool doVisu = false;


bool FrameInfoListener::process(const Frame& in)
{
    slots.clear();
    in.info(slots);

    fc = in.optUInt("fc", 0);
    ledTemp = in.optFloat("lt", -1);
    mainTemp = in.optFloat("mt", -1);

    cout << "FrameInfoListener::process() " << ledTemp << endl;
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
