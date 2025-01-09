#include <iostream>
#include <string>
#include <thread>

#include <opencv2/highgui.hpp>

#include <toffy/filterfactory.hpp>
#include <toffy/player.hpp>
#include <toffy/bta/bta.hpp>

#include "toffy_oatpp/globals.hpp"

#include "toffy_oatpp/filters/webAdapter.hpp"
#include "toffy_oatpp/webapi/webApp.hpp"
#include "toffy_oatpp/webapi/controller/btaAdapterController.hpp"
#include "toffy_oatpp/webapi/controller/frameInfoController.hpp"

using namespace std;
using namespace toffy;
using namespace toffy::webapi;

/**
 *  main
 */
int main(int argc, const char *argv[])
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


    toffy::Player *player;
    player = new toffy::Player(boost::log::trivial::debug, false);

    player->loadConfig(confFile);

    // WEB FRONTEND:

    bool enableSwaggerUi =
        getenv("WEBAPI") ? getenv("WEBAPI")[0] == '1' : true;

    setupGlobalState(getGlobalState(), enableSwaggerUi, player);

    webAppStart(getGlobalState());

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
    webAppStop();
    cout << "done" << endl;

    return 0;
}