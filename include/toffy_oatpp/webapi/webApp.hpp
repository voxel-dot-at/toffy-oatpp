#ifndef WEB_APP_H_
#define WEB_APP_H_

#include "oatpp/web/server/api/ApiController.hpp"

class WebApiState;

namespace toffy {
namespace webapi {

class WebAdapter;

// call this first...
extern void webAppInit(); 


extern void registerController(std::shared_ptr<oatpp::web::server::api::ApiController> controller);


extern void webAppStart(WebApiState& state);

extern void webAppStart(toffy::webapi::WebAdapter* webAdap,
                        WebApiState* state = nullptr);
extern void webAppStop();

}  // namespace oatpp
}  // namespace toffy

#endif