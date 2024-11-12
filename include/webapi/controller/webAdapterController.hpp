#ifndef WEB_ADAP_CONTROLLER_HPP
#define WEB_ADAP_CONTROLLER_HPP

#include <string>
#include <map>

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/web/protocol/http/outgoing/MultipartBody.hpp"
#include "oatpp/base/Log.hpp"

#include "webapi/dto/webAdapterDto.hpp"

#include "oatpp/data/resource/Resource.hpp"
#include "oatpp/async/Executor.hpp"
#include "oatpp/async/ConditionVariable.hpp"

#include "webapi/filters/webAdapter.hpp"

#include "toffy/frame.hpp"

#include "globals.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)  //<-- Begin Codegen

/**
 * Sample Api Controller.
 */
class WebAdapterController : public oatpp::web::server::api::ApiController
{
    toffy::webapi::WebAdapter* webAdap;

    // std::shared_ptr<oatpp::data::resource::Resource> m_resource;
    // oatpp::async::LockGuard m_lockGuard;
    // oatpp::async::ConditionVariable* m_cv;

   public:
    /**
   * Constructor with object mapper.
   * @param apiContentMappers - mappers used to serialize/deserialize `s.
   */
    WebAdapterController(OATPP_COMPONENT(
        std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
    }

    void registerCallbacks(toffy::webapi::WebAdapter* webApi);

   public:
    ENDPOINT_INFO(getFr)
    {
        info->summary = "Get FR";
        info->description = "Get FR";
    }
    ENDPOINT("GET", "/api/fr", getFr)
    {
        OATPP_LOGd("Test", "api/fr");
        if (!webAdap) {
            return createResponse(Status::CODE_404, "no web adapter");
        }
        webAdap->sharedMat = nullptr;
        OATPP_LOGd("Test", "api/fr waity");
        {
            oatpp::async::LockGuard guard(&webAdap->shared_lock);

            webAdap->shared_cv.wait(guard, [this]() noexcept {
                return webAdap->sharedMat != nullptr;
            });
        }
        OATPP_LOGd("Test", "api/fr waited");

        return createResponse(Status::CODE_200, "released");
    }

    ENDPOINT_INFO(getFrameInfo)
    {
        info->summary = "Get Frame Info";
        info->description = "Get Frame Info";
    }
    ENDPOINT("GET", "/api/frame", getFrameInfo)
    {
        OATPP_LOGd("Test", "api/frame");
        if (!webAdap) {
            return createResponse(Status::CODE_404, "no web adapter");
        }
        auto dto = FrameDto::createShared();
        dto->fc = webAdap->fc;

        // toffy::webapi::FrameInfoListener* fisi = nullptr;
        // const std::string f = "frameInfo";
        // auto iter = lisis.find(f);
        // if (iter == lisis.end()) {
        //     fisi = new toffy::webapi::FrameInfoListener(f);
        //     lisis.insert({f, fisi});
        //     webAdap->listeners.push_back(fisi);
        // } else {
        //     fisi = (toffy::webapi::FrameInfoListener*)(iter->second);
        // }
        // fisi->waitForWork();

        toffy::webapi::FrameInfoListener filo("frame");
        auto a = webAdap->fetchNextFrame(&filo);

        toffy::webapi::FrameInfoListener* fisi = &filo;
        dto->fc = fisi->fc;
        dto->ts = fisi->ts;
        dto->ledTemp = fisi->ledTemp;
        dto->mainTemp = fisi->mainTemp;

        dto->slots = {};
        for (auto iter = fisi->slots.begin(); iter != fisi->slots.end();
             iter++) {
            auto slot = SlotInfoDto::createShared();
            slot->name = iter->key;
            slot->dt = iter->dt;
            slot->desc = iter->description;
            dto->slots->push_back(slot);

            if (iter->dt == toffy::Frame::SlotDataType::Mat) {
                dto->mats->push_back(slot);
            }
        }

        // dto->fields = {};
        // dto->dts = {};
        // dto->desc = {};
        // for (auto iter = fisi->slots.begin(); iter != fisi->slots.end();
        //      iter++) {
        //     dto->fields->push_back(iter->key);
        //     dto->dts->push_back(iter->dt);
        //     dto->desc->push_back(iter->description);
        // }
        return createDtoResponse(Status::CODE_200, dto);
    }

    std::map<std::string, toffy::webapi::WebListener*> lisis;

    ENDPOINT_INFO(getImage)
    {
        info->summary = "Get Image";
        info->description = "Get Image";
    }
    ENDPOINT("GET", "/api/frame/{field}/img.jpg", getImage, PATH(String, field))
    {
        OATPP_LOGd("Test", "api/frame/{}/img.jpg", field);
        std::string f = field;

        if (!webAdap) {
            return createResponse(Status::CODE_404, "no web adapter");
        }
        toffy::webapi::ImgListener* lisi = nullptr;
        if (lisis.find(f) == lisis.end()) {
            lisi = new toffy::webapi::ImgListener(f);
            lisis.insert({f, lisi});
            webAdap->listeners.push_back(lisi);
        }
        lisi = (toffy::webapi::ImgListener*)(lisis.find(f)->second);
        lisi->waitForWork();
        if (lisi->valid) {
            std::string& j = *lisi->strVal;
            oatpp::String str(j.data(), j.size());

            auto resp = createResponse(Status::CODE_200, str);
            resp->putHeader(Header::CONTENT_TYPE, "image/jpeg");
            return resp;
        }
        return createResponse(Status::CODE_404);
    }
};

#include OATPP_CODEGEN_END(ApiController)  //<-- End Codegen

#endif /* MyController_hpp */