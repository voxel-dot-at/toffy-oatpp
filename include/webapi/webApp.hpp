#ifndef WEB_APP_H_
#define WEB_APP_H_

class SystemState;

namespace toffy {
namespace webapi {

class WebAdapter;

extern void webAppStart(toffy::webapi::WebAdapter* webAdap,
                        SystemState* state = nullptr);
extern void webAppStop();

}  // namespace oatpp
}  // namespace toffy

#endif