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

#include <iostream>
#include <string>
#include <thread>

#include <opencv2/highgui.hpp>

#include <toffy/filterfactory.hpp>
#include <toffy/player.hpp>
#include <toffy/bta/bta.hpp>

#include "toffy_oatpp/globals.hpp"

#include "toffy_oatpp/toffy/webAdapter.hpp"
#include "toffy_oatpp/webapi/webApp.hpp"
#include "toffy_oatpp/webapi/controller/btaAdapterController.hpp"
#include "toffy_oatpp/webapi/controller/frameInfoController.hpp"

using namespace std;
using namespace toffy_oatpp;
using namespace toffy_oatpp::webapi;

/**
 *  main
 */
int main(int argc, const char* argv[])
{
    cout << "starting" << endl;
    std::string confFile = "xml/p230_depth_ampl.xml";

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-c") {
            // parse config file
            cout << "ARGV parsing config " << argv[i + 1] << endl;
            confFile = argv[i + 1];
            i++;
        }
    }

    {
        //may return 0 when not able to detect
        const auto processor_count = std::thread::hardware_concurrency();
        cout << "# cores detected " << processor_count << endl;
    }

    toffy::Player* player;
    player = new toffy::Player(boost::log::trivial::debug, false);

    player->loadConfig(confFile);

    // WEB FRONTEND:
    bool enableSwaggerUi = getenv("WEBAPI") ? getenv("WEBAPI")[0] == '1' : true;

    setupGlobalState(getGlobalState(), enableSwaggerUi, player);

    webApiStart(getGlobalState());

    // END WEB FRONTEND

    bool keepRunning = true;
    int delay = 1;
    do {
        player->runOnce();

        int c = cv::waitKey(delay);

        // checkFc might have set it to false:
        keepRunning = keepRunning && (c != 'q');

    } while (keepRunning);

    cout << "stopping" << endl;
    webApiStop();
    cout << "done" << endl;

    return 0;
}
