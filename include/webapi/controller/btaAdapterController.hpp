#pragma once

#include <stdlib.h>

#include <string>
#include <map>

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/base/Log.hpp"

#include "webapi/dto/btaDto.hpp"

#include "toffy/bta/bta.hpp"
#include "toffy/bta/BtaWrapper.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)  //<-- Begin Codegen

/**
 * Sample Api Controller for the Bta.
 */
class BtaAdapterController : public oatpp::web::server::api::ApiController
{
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> om;

    toffy::capturers::Bta* bta;

   public:
    /**
   * Constructor with object om->
   * @param apiContentMappers - mappers used to serialize/deserialize `s.
   */
    BtaAdapterController(OATPP_COMPONENT(
        std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
        om = apiContentMappers->getDefaultMapper();
        OATPP_LOGd("CAMH", "CameraController: mapper serializing to {}/{}", om->getInfo().mimeType,
                   om->getInfo().mimeSubtype);
    }

    void registerBta(toffy::capturers::Bta* bta) { this->bta = bta; };

   public:
    ENDPOINT_INFO(GetBtaInfo)
    {
        info->summary = "Get Camera Settings";
        info->description = "Get Camera Settings";
        info->addResponse<Object<BtaSettingsDTO>>(Status::CODE_200, "application/json");
    }
    ENDPOINT_ASYNC("GET", "/api/bta", GetBtaInfo)
    {
        ENDPOINT_ASYNC_INIT(GetBtaInfo)

        Action act() override
        {
            OATPP_LOGd("BTA", "GET api/bta");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            BtaWrapper* s = self->bta->getSensor();
            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            auto response = self->createDtoResponse(Status::CODE_200, self->populateBtaSettingsDTO(*s));

            return _return(response);
        }
    };

    inline BtaSettingsDTO::Wrapper populateBtaSettingsDTO(BtaWrapper& bta)
    {
        auto dto = BtaSettingsDTO::createShared();

        dto->connected = bta.getState() == BtaWrapper::ConnState::connected;
        dto->integrationTime = bta.getIntegrationTime();
        dto->modulationFrequency = bta.getModulationFrequency();
        dto->frameRate = bta.getFrameRate();
        dto->globalOffset = bta.getGlobalOffset();
        return dto;
    }

    ENDPOINT_INFO(GetIntegrationTime)
    {
        info->summary = "Get Integration Time";
        info->description = "Get Integration Time";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }
    ENDPOINT_ASYNC("GET", "/api/bta/integrationTime", GetIntegrationTime)
    {
        ENDPOINT_ASYNC_INIT(GetIntegrationTime)

        Action act() override
        {
            OATPP_LOGd("BTA", "GET api/bta/integrationTime");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            BtaWrapper* s = self->bta->getSensor();
            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            unsigned int value = s->getIntegrationTime();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Int32(value));

            return _return(response);
        }
    };

    ENDPOINT_INFO(SetIntegrationTime)
    {
        info->summary = "Set Integration Time";
        info->description = "Set Integration Time";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
        info->pathParams.add<Int32>("time").description = "input a time value";
    }
    ENDPOINT_ASYNC("POST", "/api/bta/integrationTime/{time}", SetIntegrationTime)
    {
        ENDPOINT_ASYNC_INIT(SetIntegrationTime)

        Action act() override
        {
            OATPP_LOGd("BTA", "POST api/bta/integrationTime");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            auto timeStr = request->getPathVariable("time");
            BtaWrapper* s = self->bta->getSensor();

            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            try {
                int time = std::stoi(timeStr->c_str());

                if (time > 0) {
                    int result = s->setIntegrationTime(time);

                    if (result == 0) {
                        return _return(self->createResponse(Status::CODE_406, "Invalid integration time"));
                    }
                } else {
                    return _return(self->createResponse(Status::CODE_400, "Integration time must be greater than 0"));
                }
            } catch (const std::exception& e) {
                return _return(self->createResponse(Status::CODE_400, "Invalid time format"));
            }

            unsigned int value = s->getIntegrationTime();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Int32(value));

            return _return(response);
        }
    };

    ENDPOINT_INFO(GetFrameRate)
    {
        info->summary = "Get Frame Rate";
        info->description = "Get Frame Rate";
        info->addResponse<Float32>(Status::CODE_200, "application/json");
    }
    ENDPOINT_ASYNC("GET", "/api/bta/frameRate", GetFrameRate)
    {
        ENDPOINT_ASYNC_INIT(GetFrameRate)

        Action act() override
        {
            OATPP_LOGd("BTA", "GET api/bta/frameRate");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            BtaWrapper* s = self->bta->getSensor();
            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            float value = s->getFrameRate();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Float32(value));

            return _return(response);
        }
    };

    ENDPOINT_INFO(SetFrameRate)
    {
        info->summary = "Set Frame Rate";
        info->description = "Set Frame Rate";
        info->addResponse<Float32>(Status::CODE_200, "application/json");
        info->pathParams.add<Float32>("rate").description = "input a rate value";
    }
    ENDPOINT_ASYNC("POST", "/api/bta/frameRate/{rate}", SetFrameRate)
    {
        ENDPOINT_ASYNC_INIT(SetFrameRate)

        Action act() override
        {
            OATPP_LOGd("BTA", "POST api/bta/frameRate");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            auto rateStr = request->getPathVariable("rate");
            BtaWrapper* s = self->bta->getSensor();

            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            try {
                float rate = std::stoi(rateStr->c_str());

                if (rate > 0) {
                    int result = s->setFrameRate(rate);

                    if (result == 0) {
                        return _return(self->createResponse(Status::CODE_406, "Invalid frame rate"));
                    }
                } else {
                    return _return(self->createResponse(Status::CODE_400, "Frame rate must be greater than 0"));
                }
            } catch (const std::exception& e) {
                return _return(self->createResponse(Status::CODE_400, "Invalid frame rate format"));
            }

            float value = s->getFrameRate();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Float32(value));

            return _return(response);
        }
    };

    ENDPOINT_INFO(GetModulationFrequency)
    {
        info->summary = "Get Modulation Frequency";
        info->description = "Get Modulation Frequency";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }
    ENDPOINT_ASYNC("GET", "/api/bta/modulationFrequency", GetModulationFrequency)
    {
        ENDPOINT_ASYNC_INIT(GetModulationFrequency)

        Action act() override
        {
            OATPP_LOGd("BTA", "GET api/bta/modulationFrequency");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            BtaWrapper* s = self->bta->getSensor();
            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            unsigned int value = s->getModulationFrequency();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Int32(value));
            return _return(response);
        }
    };

    ENDPOINT_INFO(SetModulationFrequency)
    {
        info->summary = "Set Modulation Frequency";
        info->description = "Set Modulation Frequency";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
        info->pathParams.add<Int32>("freq").description = "input a modulation frequency";
    }
    ENDPOINT_ASYNC("POST", "/api/bta/modulationFrequency/{freq}", SetModulationFrequency)
    {
        ENDPOINT_ASYNC_INIT(SetModulationFrequency)

        Action act() override
        {
            OATPP_LOGd("BTA", "POST api/bta/modulationFrequency");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            auto freqStr = request->getPathVariable("freq");
            BtaWrapper* s = self->bta->getSensor();

            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            try {
                int freq = std::stoi(freqStr->c_str());

                if (freq > 0) {
                    int result = s->setModulationFrequency(freq);

                    if (result == 0) {
                        return _return(self->createResponse(Status::CODE_406, "Invalid modulation frequency"));
                    }
                } else {
                    return _return(self->createResponse(Status::CODE_400, "Invalid modulation frequency"));
                }
            } catch (const std::exception& e) {
                return _return(self->createResponse(Status::CODE_400, "Invalid modulation frequency"));
            }

            unsigned int value = s->getModulationFrequency();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Int32(value));

            return _return(response);
        }
    };

    ENDPOINT_INFO(GetGlobalOffset)
    {
        info->summary = "Get Global Offset";
        info->description = "Get Global Offset";
        info->addResponse<Float32>(Status::CODE_200, "application/json");
    }
    ENDPOINT_ASYNC("GET", "/api/bta/globalOffset", GetGlobalOffset)
    {
        ENDPOINT_ASYNC_INIT(GetGlobalOffset)

        Action act() override
        {
            OATPP_LOGd("BTA", "GET api/bta/globalOffset");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            BtaWrapper* s = self->bta->getSensor();
            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            float value = s->getGlobalOffset();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Float32(value));

            return _return(response);
        }
    };

    ENDPOINT_INFO(SetGlobalOffset)
    {
        info->summary = "Set Global Offset";
        info->description = "Set Global Offset";
        info->addResponse<Float32>(Status::CODE_200, "application/json");
        info->pathParams.add<Float32>("offset").description = "input a modulation frequency";
    }
    ENDPOINT_ASYNC("POST", "/api/bta/modulationFrequency/{offset}", SetGlobalOffset)
    {
        ENDPOINT_ASYNC_INIT(SetGlobalOffset)

        Action act() override
        {
            OATPP_LOGd("BTA", "POST api/bta/modulationFrequency");
            BtaAdapterController* self = (BtaAdapterController*)controller;
            auto offsetStr = request->getPathVariable("offset");
            BtaWrapper* s = self->bta->getSensor();

            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            try {
                float offset = std::stoi(offsetStr->c_str());

                if (offset > 0) {
                    int result = s->setGlobalOffset(offset);

                    if (result == 0) {
                        return _return(self->createResponse(Status::CODE_406, "Invalid global offset"));
                    }
                } else {
                    return _return(self->createResponse(Status::CODE_400, "Invalid global offset"));
                }
            } catch (const std::exception& e) {
                return _return(self->createResponse(Status::CODE_400, "Invalid global offset"));
            }

            float value = s->getGlobalOffset();
            auto response = self->createDtoResponse(Status::CODE_200, oatpp::Float32(value));

            return _return(response);
        }
    };

    ENDPOINT_INFO(GetRegister)
    {
        info->summary = "Get Register";
        info->description = "Return value is integer decimal not hexadecimal";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
        info->pathParams.add<String>("addr").description = "input can be decimal (123) or hexadecimal (0xabc)";
    }
    ENDPOINT_ASYNC("GET", "/api/bta/reg/{addr}", GetRegister)
    {
        ENDPOINT_ASYNC_INIT(GetRegister)

        Action act() override
        {
            OATPP_LOGd("BTA", "GET /api/bta/reg");
            BtaAdapterController* self = (BtaAdapterController*)controller;

            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            try {
                auto a = request->getPathVariable("addr");
                char* endptr = NULL;

                long int reg = strtol(((std::string)a).c_str(), &endptr, 0);

                if (*endptr != '\0' || reg < 0) {
                    OATPP_LOGd("BTA", "Invalid register address: %s", a->c_str());
                    return _return(self->createResponse(Status::CODE_400, "Invalid register address"));
                }

                BtaWrapper* s = self->bta->getSensor();
                unsigned int value = s->readRegister(reg);

                auto response = self->createDtoResponse(Status::CODE_200, oatpp::Int32(value));
                return _return(response);

            } catch (const std::exception& e) {
                OATPP_LOGe("BTA", "Exception occurred: %s", e.what());
                return _return(self->createResponse(Status::CODE_400, "Error processing register address"));
            }
        }
    };

    ENDPOINT_INFO(SetRegister)
    {
        info->summary = "Set Register";
        info->description = "Return value is integer decimal not hexadecimal";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
        info->pathParams.add<String>("addr").description = "input can be decimal (123) or hexadecimal (0xabc)";
        info->pathParams.add<String>("value").description = "input can be decimal (123) or hexadecimal (0xabc)";
    }
    ENDPOINT_ASYNC("POST", "/api/bta/reg/{addr}/{value}", SetRegister)
    {
        ENDPOINT_ASYNC_INIT(SetRegister)

        Action act() override
        {
            OATPP_LOGd("BTA", "POST /api/bta/reg");
            BtaAdapterController* self = (BtaAdapterController*)controller;

            if (!self->bta) {
                return _return(self->createResponse(Status::CODE_404, "No BTA available"));
            }

            try {
                auto a = request->getPathVariable("addr");
                auto v = request->getPathVariable("value");
                char* endptr = NULL;

                long int reg = strtol(((std::string)a).c_str(), &endptr, 0);

                if (*endptr != '\0' || reg < 0) {
                    OATPP_LOGd("BTA", "Invalid register address: %s", a->c_str());
                    return _return(self->createResponse(Status::CODE_400, "Invalid register address"));
                }

                long int val = strtol(((std::string)v).c_str(), &endptr, 0);

                if (*endptr != '\0') {
                    OATPP_LOGd("BTA", "Invalid register value: %s", v->c_str());
                    return _return(self->createResponse(Status::CODE_400, "Invalid register value"));
                }

                BtaWrapper* s = self->bta->getSensor();
                s->writeRegister(reg, val);

                unsigned int value = s->readRegister(reg);

                auto response = self->createDtoResponse(Status::CODE_200, oatpp::Int32(value));
                return _return(response);

            } catch (const std::exception& e) {
                OATPP_LOGe("BTA", "Exception occurred: %s", e.what());
                return _return(self->createResponse(Status::CODE_400, "Error processing register address or value"));
            }
        }
    };

   private:
    // std::shared_ptr<Response> upFreq(float freq);

    void populateBtaSettings(BtaSettingsDTO& settings);
};

#include OATPP_CODEGEN_END(ApiController)  //<-- End Codegen
