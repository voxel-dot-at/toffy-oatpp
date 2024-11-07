#include <string>
#include <iostream>

#include <opencv2/highgui.hpp>

#include "globals.hpp"
#include "util.hpp"

#include "webapi/filters/webAdapter.hpp"

#include "webapi/webApp.hpp"
#include "webapi/webAppComponent.hpp"

#include <iostream>

using namespace std;
using namespace toffy::webapi;

bool init_toffy_web()
{
    toffy::FilterFactory* ff = toffy::FilterFactory::getInstance();

    ff->registerCreator("webAdapter", toffy::webapi::CreateWebAdapter);

    return true;
}


std::vector<std::string> ids = {"1234", "12345", "132456", "1234455"};
int idx = 0;


toffy::detection::Measurement* newMeas()
{
    toffy::detection::Measurement* m = new toffy::detection::Measurement();
    m->avgH = idx / 1.23;
    m->id = ids[idx++];
    m->state = toffy::detection::Measurement::success;
    return m;
}


/**
 *  main
 */
int main(int argc, const char* argv[])
{
    cout << "starting" << endl;

    init_toffy_web();

    toffy::webapi::WebAdapter* webAdap = new toffy::webapi::WebAdapter();

    theState.store.add(newMeas());
    theState.store.add(newMeas());
    theState.store.add(newMeas());
    theState.store.print();

    toffy::Player* player;
    player = new toffy::Player(boost::log::trivial::debug, false);
    player->loadConfig("xml/oatpp.xml");

    player->filterBank()->add(webAdap);

    SystemState state;
    setupFilterBank(state,player->filterBank());

    webAppStart(webAdap, &state);

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