#ifndef WEB_ADAP_CONTROLLER_HPP
#define WEB_ADAP_CONTROLLER_HPP

#include <string>
#include <iostream>
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
    toffy::webapi::FrameInfoListener filo;

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
    ENDPOINT_ASYNC("GET", "/api/g", FrameCo)
    {
        ENDPOINT_ASYNC_INIT(FrameCo);

        Action act() override
        {
            std::cout << "gGGGGG " << std::endl;
            WebAdapterController* self = (WebAdapterController*)controller;
            toffy::webapi::WebAdapter* webAdap = self->webAdap;
            toffy::webapi::FrameInfoListener* fisi = &self->filo;
            std::cout << "webAdap pre lk# " << webAdap->resource.counter
                      << " " << fisi->name() <<  std::endl;
            webAdap->singleShot(fisi);
            std::cout << __LINE__ << "fifi" << std::endl;
            oatpp::async::LockGuard lockGuard(&webAdap->theLock());

            std::cout << __LINE__ << "fifi" << std::endl;
            return webAdap->theCv()
                .wait(lockGuard,
                      [self] { 
                        std::cout << __LINE__ << "fnrf? " << std::endl;
                        return self->webAdap->resource.counter > 0;
                         })
                .next(yieldTo(&FrameCo::onReady));
        }

        Action onReady()
        {
            WebAdapterController* self = (WebAdapterController*)controller;
            std::cout << __LINE__ << "fifi" << std::endl;
            toffy::webapi::WebAdapter* webAdap = self->webAdap;
            toffy::webapi::FrameInfoListener* fisi = &self->filo;
            std::cout << __LINE__ << "fifi" << std::endl;
            // OATPP_ASSERT(
            //     m_lockGuard.owns_lock())  // Now coroutine owns the lock

            std::cout << "webAdap onReady# " << webAdap->resource.counter
                      << std::endl;
            auto dto = FrameDto::createShared();
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

            dto->fields = {};
            // dto->dts = {};
            // dto->desc = {};
            for (auto iter = fisi->slots.begin(); iter != fisi->slots.end();
                 iter++) {
                //     dto->fields->push_back(iter->key);
                //     dto->dts->push_back(iter->dt);
                //     dto->desc->push_back(iter->description);
            }
            return _return(
                controller->createDtoResponse(Status::CODE_200, dto));
            // return finish();
        }
    };

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

        toffy::webapi::FrameInfoListener* fisi = &filo;
        std::cout << "webAdap pre lk# " << webAdap->resource.counter
                  << std::endl;
        webAdap->singleShot(&filo);
        {
            oatpp::async::LockGuard lockGuard(&webAdap->theLock());
            std::chrono::duration<v_int64, std::micro> timeout(10);

            webAdap->theCv().waitFor(
                lockGuard,
                [this]() {
                    std::cout << "webAdap in lk# " << webAdap->resource.counter
                              << std::endl;
                    return webAdap->resource.counter > 0;
                },
                timeout)
                // .next(&WebAdapterController::processFrame)
                ;
            std::cout << "webAdap pst lk# " << webAdap->resource.counter
                      << std::endl;

            webAdap->resource.counter--;
        }

        std::cout << "webAdap pst delta fc " << webAdap->fc << " " << fisi->fc
                  << std::endl;
        auto dto = FrameDto::createShared();
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

        dto->fields = {};
        // dto->dts = {};
        // dto->desc = {};
        for (auto iter = fisi->slots.begin(); iter != fisi->slots.end();
             iter++) {
            //     dto->fields->push_back(iter->key);
            //     dto->dts->push_back(iter->dt);
            //     dto->desc->push_back(iter->description);
        }
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