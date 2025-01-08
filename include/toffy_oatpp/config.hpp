#pragma once

#include <string>

namespace toffy {
namespace webapi {

/** collection of configuration parameters for the web api.
 * 
 */
class ConfigParams
{
    unsigned int port = 8000;

    std::string prefix = "./web/";
};

}  // namespace webapi
}  // namespace toffy
