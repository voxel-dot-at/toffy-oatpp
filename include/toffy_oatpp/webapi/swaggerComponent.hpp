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

#ifndef SwaggerComponent_hpp
#define SwaggerComponent_hpp

#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"
#include "oatpp/macro/component.hpp"

/**
 *  Swagger ui is served at
 *  http://host:port/swagger/ui
 */
class SwaggerComponent
{
   public:
    /**
   *  General API docs info
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>,
                           swaggerDocumentInfo)
    ([] {
        oatpp::swagger::DocumentInfo::Builder builder;

        builder.setTitle("Camera control api")
            .setDescription("BTA Camera control api")
            .setVersion("1.0")
            .setContactName("Simon Vogl")
            .setContactUrl("https://voxel.at/")

            .setLicenseName("Apache License, Version 2.0")
            .setLicenseUrl("http://www.apache.org/licenses/LICENSE-2.0")

            .addServer("http://localhost:8000", "server on localhost");
            // .addServer("http://192.168.2.133:8000", "server on lab.net");

        return builder.build();
    }());

    /**
   *  Swagger-Ui Resources (<oatpp-examples>/lib/oatpp-swagger/res)
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>,
                           swaggerResources)
    ([] {
        // Make sure to specify correct full path to oatpp-swagger/res folder !!!
        return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
    }());
};

#endif /* SwaggerComponent_hpp */