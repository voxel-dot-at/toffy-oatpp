#include <iostream>
#include <string>

#include <opencv2/highgui.hpp>

#include <toffy/filterfactory.hpp>
#include <toffy/player.hpp>
#include <toffy/bta/bta.hpp>

#include "globals.hpp"

#include "webapi/filters/webAdapter.hpp"
#include "webapi/webApp.hpp"
#include "webapi/webAppComponent.hpp"

using namespace std;
using namespace toffy;
using namespace toffy::webapi;

extern SystemState theState;

bool init_toffy_web()
{
    toffy::FilterFactory *ff = toffy::FilterFactory::getInstance();

    ff->registerCreator("webAdapter", toffy::webapi::CreateWebAdapter);

    return true;
}

void setupGlobalState(SystemState &state, Player* player)
{
    state.player = player;

    state.enableSwaggerUi = getenv("WEBAPI") ? getenv("WEBAPI")[0] == '1' : true; 

    FilterBank *fb = player->filterBank();

    const std::string BTA_NAME = "bta";

    {  // find bta
        std::vector<Filter *> vec;
        fb->getFiltersByType(BTA_NAME, vec);
        for (unsigned int i = 0; i < vec.size(); i++) {
            toffy::capturers::Bta *c = (toffy::capturers::Bta *)vec[i];
            cout << "setupFilterBank found bta: " << c->name() << endl;
            theState.bta = c;
        }
        if (vec.size() == 0) {
            cout << "WARNING! NO FILTER TYPE  " << BTA_NAME << " FOUND!" << endl;
        }
    }
}

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

    init_toffy_web();


    toffy::Player *player;
    player = new toffy::Player(boost::log::trivial::debug, false);

    player->loadConfig(confFile);

    setupGlobalState(theState, player);

    // web api:::
    // initialize web api linkage:
    toffy::webapi::WebAdapter *webAdap = new toffy::webapi::WebAdapter();
    player->filterBank()->add(webAdap);

    webAppStart(webAdap, &theState);
    // /web api


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