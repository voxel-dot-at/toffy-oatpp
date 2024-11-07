#include <string>
#include <iostream>

#include <opencv2/highgui.hpp>

#include "globals.hpp"

#include "webapi/filters/webAdapter.hpp"

#include "webapi/webApp.hpp"
#include "webapi/webAppComponent.hpp"

#include <iostream>

using namespace std;
using namespace toffy::webapi;


extern SystemState theState;


bool init_toffy_web()
{
    toffy::FilterFactory* ff = toffy::FilterFactory::getInstance();

    ff->registerCreator("webAdapter", toffy::webapi::CreateWebAdapter);

    return true;
}

/**
 *  main
 */
int main(int argc, const char* argv[])
{
    cout << "starting" << endl;

    init_toffy_web();

    toffy::webapi::WebAdapter* webAdap = new toffy::webapi::WebAdapter();

    toffy::Player* player;
    player = new toffy::Player(boost::log::trivial::debug, false);
    player->loadConfig("xml/oatpp.xml");

    player->filterBank()->add(webAdap);

    setupFilterBank(state,player->filterBank());

    webAppStart(webAdap, &theState);

    bool keepRunning = true;
    int delay = 1;
    do {
        player->runOnce();

        int c = cv::waitKey(delay);

        // checkFc might have set it to false:
        keepRunning = keepRunning && (c != 'q');

    } while (keepRunning);


    sleep(360);
    cout << "stopping" << endl;
    webAppStop();
    cout << "done" << endl;

    return 0;
}