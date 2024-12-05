#ifndef WEB_APP_H_
#define WEB_APP_H_

#include "oatpp/web/server/api/ApiController.hpp"
class SystemState;

namespace toffy {
namespace webapi {

class WebAdapter;


extern void registerController(std::shared_ptr<oatpp::web::server::api::ApiController> controller);


extern void webAppStart(toffy::webapi::WebAdapter* webAdap,
                        SystemState* state = nullptr);
extern void webAppStop();

}  // namespace oatpp
}  // namespace toffy

#endif