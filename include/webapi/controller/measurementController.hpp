#ifndef MEAS_CONTROLLER_HPP
#define MEAS_CONTROLLER_HPP

#include "webapi/dto/testDto.hpp"
#include "webapi/dto/measurementDto.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/base/Log.hpp"
#include "globals.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)  //<-- Begin Codegen

/**
 * Sample Api Controller.
 */
class MeasurementController : public oatpp::web::server::api::ApiController
{
   public:
    /**
   * Constructor with object mapper.
   * @param apiContentMappers - mappers used to serialize/deserialize `s.
   */
    MeasurementController(OATPP_COMPONENT(
        std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
    }

   public:
    ENDPOINT_INFO(getMeasurements)
    {
        info->summary = "get measurements";
        info->description = "get a list of measurement IDs.";

        info->addConsumes<String>("application/json");
        info->addResponse<List<String>>(Status::CODE_200, "application/json",
                                        "list of measurement IDs");
    }
    ENDPOINT("GET", "/api/measurements", getMeasurements,
             HEADER(String, userAgent, "User-Agent"))
    {
        OATPP_LOGd("Test", "api/measurements userAgent={}", userAgent);
        auto dto = MeasurementsDto::createShared();
        MeasurementStore& s = theState.store;

        dto->ids = {};  // init, otherwise it is null

        auto iter = s.begin();
        while (iter != s.end()) {
            toffy::detection::Measurement* m = *iter;
            dto->ids->push_back(m->id);
            iter++;
        }
        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT_INFO(getMeasurementById)
    {
        info->summary = "get measurement data";
        info->description =
            "get a detailed measurement result by passing the measurment ID.";

        info->addConsumes<String>("application/json");
        info->addResponse<Object<MeasurementDto>>(Status::CODE_200,
                                                  "application/json");
    }
    ENDPOINT("GET", "/api/measurement/{mid}", getMeasurementById,
             PATH(String, mid))
    {
        MeasurementStore& s = theState.store;
        toffy::detection::Measurement* m = s.findById(mid);
        OATPP_LOGd("Test", "api/measurement/{} f? {}", mid, m != nullptr);

        if (m) {
            auto dto = MeasurementDto::createShared();
            dto->id = m->id;
            dto->barcode = m->barcode;
            dto->state = m->state;
            dto->errorMsg = m->errorMsg;
            dto->hasRgb = m->jpegData != nullptr;
            dto->hasTof = m->tofData != nullptr;

            dto->boxWidth = m->boxWidth;
            dto->boxLength = m->boxLength;
            dto->boxHeight = m->boxHeight;

            dto->width = m->width;
            dto->length = m->length;
            dto->height = m->height;

            dto->avgH = m->avgH;
            dto->maxH = m->maxH;
            return createDtoResponse(Status::CODE_200, dto);
        } else {
            return createResponse(Status::CODE_404, "not found.");
        }
    }

    ENDPOINT("GET", "/api/measurement/{mid}/rgb", getRgbById, PATH(String, mid))
    {
        MeasurementStore& s = theState.store;
        toffy::detection::Measurement* m = s.findById(mid);

        if (m && m->jpegData) {
            std::string& j = (std::string&)*m->jpegData;
            oatpp::String str(j.data(), j.size());

            auto resp = createResponse(Status::CODE_200, str);
            resp->putHeader(Header::CONTENT_TYPE, "image/jpeg");
            return resp;
        } else {
            return createResponse(Status::CODE_404, "not found.");
        }
    }
};

#include OATPP_CODEGEN_END(ApiController)  //<-- End Codegen

#endif /* MyController_hpp */