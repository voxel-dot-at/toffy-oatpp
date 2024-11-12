#ifndef BTA_ADAP_CONTROLLER_HPP
#define BTA_ADAP_CONTROLLER_HPP

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
    oatpp::json::ObjectMapper mapper;

    toffy::capturers::Bta* bta;

   public:
    /**
   * Constructor with object mapper.
   * @param apiContentMappers - mappers used to serialize/deserialize `s.
   */
    BtaAdapterController(OATPP_COMPONENT(
        std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
    }

    void registerBta(toffy::capturers::Bta* bta) { this->bta = bta; };

   public:
    ENDPOINT_INFO(getBtaInfo)
    {
        info->summary = "get camera settings";
        //   info->addConsumes<Object<UserDto>>("application/json");
        info->addResponse<Object<BtaSettingsDTO>>(Status::CODE_200,
                                                  "application/json");
    }
    ENDPOINT("GET", "/api/bta", getBtaInfo)
    {
        OATPP_LOGd("BTA", "api/bta");
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();

        return createDtoResponse(Status::CODE_200, populateBtaSettingsDTO(*s));
    }

    inline BtaSettingsDTO::Wrapper populateBtaSettingsDTO(BtaWrapper& bta)
    {
        auto dto = BtaSettingsDTO::createShared();
        // populateBtaSettings(dto);

        dto->connected = bta.getState() == BtaWrapper::ConnState::connected;
        dto->integrationTime = bta.getIntegrationTime();
        dto->modulationFrequency = bta.getModulationFrequency();
        dto->frameRate = bta.getFrameRate();
        dto->globalOffset = bta.getGlobalOffset();
        return dto;
    }

    // ENDPOINT_INFO(getIntegrationTime) {
    //   info->summary = "get the integration time";
    //   info->addConsumes<Object<UserDto>>("application/json");
    //   info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
    // }

    ENDPOINT("GET", "/api/bta/integrationTime", getIntegrationTime)
    {
        OATPP_LOGd("BTA", "api/bta/integrationTime G {}", time);
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();

        oatpp::Int32 tt = s->getIntegrationTime();

        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("POST", "/api/bta/integrationTime", setIntegrationTime,
             QUERY(Int32, time))
    {
        OATPP_LOGd("BTA", "api/bta/integrationTime P {}", time);
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();
        if (time > 0) {
            int result = s->setIntegrationTime((unsigned int)time);

            if (result == 0) {
                return createResponse(Status::CODE_406,
                                      "Invalid integration time");
            }
        }

        oatpp::Int32 tt = s->getIntegrationTime();
        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("GET", "/api/bta/frameRate", getFrameRate)
    {
        OATPP_LOGd("BTA", "api/bta/frameRate G");
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();

        oatpp::Float32 tt = s->getFrameRate();

        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("POST", "/api/bta/frameRate", setFrameRate, QUERY(Float32, rate))
    {
        OATPP_LOGd("BTA", "api/bta/frameRate P {}", rate);
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();
        if (rate > 0) {
            int result = s->setFrameRate((unsigned int)rate);

            if (result == 0) {
                return createResponse(Status::CODE_406, "Invalid frame rate");
            }
        }

        oatpp::Int32 tt = s->getFrameRate();
        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("GET", "/api/bta/modulationFrequency", getModulationFrequency)
    {
        OATPP_LOGd("BTA", "api/bta/modulationFrequency G");
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();

        oatpp::Int32 tt = s->getModulationFrequency();

        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("POST", "/api/bta/modulationFrequency", setModulationFrequency,
             QUERY(Float32, freq))
    {
        OATPP_LOGd("BTA", "api/bta/modulationFrequency P {}", time);
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();
        if (freq > 0) {
            int result = s->setModulationFrequency((unsigned int)freq);

            if (result == 0) {
                return createResponse(Status::CODE_406,
                                      "Invalid modulation frequency");
            }
        }

        oatpp::Int32 tt = s->getModulationFrequency();
        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("GET", "/api/bta/globalOffset", getGlobalOffset)
    {
        OATPP_LOGd("BTA", "api/bta/globalOffset G {}", time);
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();

        // nb. conversion down to int - this was in meters for old cameras, now it is mm
        oatpp::Int32 tt = s->getGlobalOffset();

        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("POST", "/api/bta/globalOffset", setGlobalOffset,
             QUERY(Float32, offset))
    {
        OATPP_LOGd("BTA", "api/bta/globalOffet P {}", time);
        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }

        BtaWrapper* s = bta->getSensor();

        int result = s->setGlobalOffset(offset);

        if (result == 0) {
            return createResponse(Status::CODE_406, "Invalid global offset");
        }

        oatpp::Int32 tt = s->getGlobalOffset();
        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("GET", "/api/bta/reg/{addr}", getRegister, PATH(String, addr))
    {
        OATPP_LOGd("BTA", "api/bta/reg G");

        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }
        BtaWrapper* s = bta->getSensor();

        std::string a = addr;
        char* endptr = NULL;
        long int reg = strtol(a.c_str(), &endptr, 0);
        unsigned int val = s->readRegister(reg);
        oatpp::Int32 tt = val;

        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

    ENDPOINT("POST", "/api/bta/reg/{addr}", postRegister, PATH(String, addr),
             QUERY(String, value))
    {
        OATPP_LOGd("BTA", "api/bta/reg P");

        if (!bta) {
            return createResponse(Status::CODE_404, "no bta");
        }
        BtaWrapper* s = bta->getSensor();

        std::string a = addr;
        std::string v = value;
        char* endptr = NULL;
        long int reg = strtol(a.c_str(), &endptr, 0);
        long int val = strtol(v.c_str(), &endptr, 0);
        s->writeRegister(reg, val);
        unsigned int vv = s->readRegister(reg);
        oatpp::Int32 tt = vv;

        return createResponse(Status::CODE_200, mapper.writeToString(tt));
    }

   private:
    // std::shared_ptr<Response> upFreq(float freq);

    void populateBtaSettings(BtaSettingsDTO& settings);
};

#include OATPP_CODEGEN_END(ApiController)  //<-- End Codegen

#endif /* MyController_hpp */