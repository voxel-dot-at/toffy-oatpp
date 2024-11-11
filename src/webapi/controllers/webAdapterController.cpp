#include "webapi/controller/webAdapterController.hpp"

void WebAdapterController::registerCallbacks(toffy::webapi::WebAdapter* webApi)
{
    this->webAdap = webApi;
}
