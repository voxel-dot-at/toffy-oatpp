#ifndef WEB_APP_H_
#define WEB_APP_H_

#include "oatpp/web/server/api/ApiController.hpp"

#include "toffy_oatpp/globals.hpp"

//class WebApiState;

namespace toffy_oatpp {
namespace webapi {

// call this first...
extern void webApiInit(); 


extern void registerController(std::shared_ptr<oatpp::web::server::api::ApiController> controller);


extern void webApiStart(WebApiState& state);

extern void webApiStop();

}  // namespace oatpp
}  // namespace toffy

#endif