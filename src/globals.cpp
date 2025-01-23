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

#include "toffy_oatpp/globals.hpp"

#include <toffy/filterfactory.hpp>
#include <toffy/player.hpp>
#include <toffy/bta/bta.hpp>

#include "toffy_oatpp/toffy/webAdapter.hpp"

#include "toffy_oatpp/webapi/webApp.hpp"
#include "toffy_oatpp/webapi/controller/btaAdapterController.hpp"
#include "toffy_oatpp/webapi/controller/frameInfoController.hpp"

using namespace std;

namespace toffy_oatpp {

static WebApiState theState;

WebApiState &getGlobalState() { return theState; }

static bool init_toffy_web()
{
    toffy::FilterFactory *ff = toffy::FilterFactory::getInstance();

    ff->registerCreator("webAdapter", toffy_oatpp::webapi::CreateWebAdapter);

    return true;
}

/** perform controller-specific initialisation before registering to the web api & starting it */
static void registerOatppControllers(WebApiState &state,
                                     webapi::WebAdapter *webAdap)
{
    auto btaAdapCtrl = std::make_shared<BtaAdapterController>();

    btaAdapCtrl->registerBta(state.bta);
    toffy_oatpp::webapi::registerController(btaAdapCtrl);

    auto fic = std::make_shared<FrameInfoController>();
    fic->setApi(&state.api);
    toffy_oatpp::webapi::registerController(fic);
}

void setupGlobalState(WebApiState &state, bool withSwagger,
                      toffy::Player *player)
{
    state.player = player;

    init_toffy_web();

    state.enableSwaggerUi = withSwagger;

    toffy::FilterBank *fb = player->filterBank();

    const std::string BTA_NAME = "bta";

    {  // find bta
        std::vector<toffy::Filter *> vec;
        fb->getFiltersByType(BTA_NAME, vec);
        for (unsigned int i = 0; i < vec.size(); i++) {
            toffy::capturers::Bta *c = (toffy::capturers::Bta *)vec[i];
            cout << "setupFilterBank found bta: " << c->name() << endl;
            theState.bta = c;
        }
        if (vec.size() == 0) {
            cout << "WARNING! NO FILTER TYPE  " << BTA_NAME << " FOUND!"
                 << endl;
        }
    }

    // initialize oatpp controller objects:
    // web api:::
    toffy_oatpp::webapi::WebAdapter *webAdap = new toffy_oatpp::webapi::WebAdapter();
    webAdap->setApi(&theState.api);
    player->filterBank()->add(webAdap);

    // initialize web api linkage:
    registerOatppControllers(theState, webAdap);
}

}  // namespace toffy_oatpp