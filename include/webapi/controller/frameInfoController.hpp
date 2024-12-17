#pragma once

#include <regex>

#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/utils/Conversion.hpp"

#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "webapi/dto/frameDto.hpp"

#include "shared/syncApi.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)  /// <-- Begin Code-Gen

class FrameInfoReadCallback;
class FrameMetaReadCallback;
class FrameJpegReadCallback;

class FrameInfoController : public oatpp::web::server::api::ApiController
{
    // for now convenience for the object mapper

    friend class FrameInfoReadCallback;
    friend class FrameMetaReadCallback;

   private:
    typedef FrameInfoController __ControllerType;

    SyncApi* api;

    std::shared_ptr<oatpp::data::mapping::ObjectMapper> om;

   private:
    // OATPP_COMPONENT(std::shared_ptr<Lobby>, lobby);

   public:
    FrameInfoController(OATPP_COMPONENT(
        std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
        om = apiContentMappers->getDefaultMapper();
        OATPP_LOGd("FIC", "FrameInfoController: mapper serializing to {}/{}",
                   om->getInfo().mimeType, om->getInfo().mimeSubtype);
    }

    void setApi(SyncApi* api) { this->api = api; }

   public:  // ENDPOINTS:
    ENDPOINT_INFO(GetFrameInfo)
    {
        info->summary = "Get Frame Info";
        info->description = "Get the next frame counter, sync via timer";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }
    ENDPOINT_ASYNC("GET", "/frame/info", GetFrameInfo)
    {
        ENDPOINT_ASYNC_INIT(GetFrameInfo);

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            auto body = std::make_shared<
                oatpp::web::protocol::http::outgoing::StreamingBody>(
                std::make_shared<FrameInfoReadCallback>(*controller->api,
                                                        controller));

            auto response =
                OutgoingResponse::createShared(Status::CODE_200, body);
            response->putHeader("content-type", "application/json");

            return _return(response);
        }
    };

    ENDPOINT_INFO(GetFrameMeta)
    {
        info->summary = "Get Frame Meta Info";
        info->description = "Get the Frame Meta Info";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }
    ENDPOINT_ASYNC("GET", "/frame/meta", GetFrameMeta)
    {
        ENDPOINT_ASYNC_INIT(GetFrameMeta);

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            auto body = std::make_shared<
                oatpp::web::protocol::http::outgoing::StreamingBody>(
                std::make_shared<FrameMetaReadCallback>(*controller->api,
                                                        controller));

            auto response =
                OutgoingResponse::createShared(Status::CODE_200, body);
            response->putHeader("content-type", "application/json");

            return _return(response);
        }
    };

    ENDPOINT_INFO(GetFrameDepth)
    {
        info->summary = "Get Frame JPEG";
        info->description = "Get the next frame as a JPEG image";
        info->pathParams.add<String>("path").description = "To get `depth` or `ampl` image, type either";
    }

    ENDPOINT_ASYNC("GET", "/frame/{path}", GetFrameDepth)
    {
        ENDPOINT_ASYNC_INIT(GetFrameDepth);

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            auto path = request->getPathVariable("path");

            auto body = std::make_shared<
                oatpp::web::protocol::http::outgoing::StreamingBody>(
                std::make_shared<FrameJpegReadCallback>(*controller->api,
                                                        controller, path));

            auto response =
                OutgoingResponse::createShared(Status::CODE_200, body);
            response->putHeader("content-type", "image/jpeg");

            return _return(response);
        }
    };
};

/** timer read callback class - spins until new data is available 
 * from the camera thread, then fills the buffer.
 * 
 */
class FrameInfoReadCallback : public oatpp::data::stream::ReadCallback
{
   private:
    SyncApi& api;
    int old = -1;
    const FrameInfoController* self;
    int len = 0;

   public:
    FrameInfoReadCallback(SyncApi& api, const FrameInfoController* self)
        : api(api), old(api.fc), self(self)
    {
    }

    oatpp::v_io_size read(void* buffer, v_buff_size count,
                          oatpp::async::Action& action) override
    {
        // timer strategy adapted from oatpp/test/oatpp/web/app/ControllerAsync.hpp
        // check if the frame counter has changed
        if (api.fc == old) {
            // limits to 50fps; better would be to compute expected arrival time by storing framerate&lastTs
            action = oatpp::async::Action::createWaitRepeatAction(
                20 * 1000 + oatpp::Environment::getMicroTickCount());
            OATPP_LOGd("CAMH", "tick");
            return 0;
        }
        OATPP_LOGd("CAMH", "ticked {} dlen={}", api.fc, len);
        if (len) {
            // second iteration with new contents - tell the body-builder we are done:
            action = oatpp::async::Action::createActionByType(
                oatpp::async::Action::TYPE_NONE);
            return 0;
        }
        // first iteration with new contents - new data available, let's prepare an answer

        auto dto = FrameDto::createShared();
        dto->fc = api.fc;
        dto->ledTemp = api.ledTemp;
        dto->mainTemp = api.mainTemp;

        oatpp::String s = self->om->writeToString(dto);

        const std::string& ss = *s;
        len = ss.size();

        memcpy(buffer, ss.c_str(), len + 1);
        return len;
    }
};

class FrameMetaReadCallback : public oatpp::data::stream::ReadCallback
{
   private:
    SyncApi& api;
    const FrameInfoController* self;
    int len = 0;

   public:
    FrameMetaReadCallback(SyncApi& api, const FrameInfoController* self)
        : api(api), self(self)
    {
    }

    oatpp::v_io_size read(void* buffer, v_buff_size count,
                          oatpp::async::Action& action) override
    {
        OATPP_LOGd("CAMH", "ticked {} dlen={}", api.fc, len);
        if (len) {
            // second iteration with new contents - tell the body-builder we are done:
            action = oatpp::async::Action::createActionByType(
                oatpp::async::Action::TYPE_NONE);
            return 0;
        }
        // first iteration with new contents - new data available, let's prepare an answer

        const toffy::Frame& frame = *api.frame;
        std::vector<toffy::Frame::SlotInfo> slots;
        frame.info(slots);

        auto dto = FrameMetaDto::createShared();
        dto->slots = {};
        for (auto iter = slots.begin(); iter != slots.end();
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

        oatpp::String s = self->om->writeToString(dto);

        const std::string& ss = *s;
        len = ss.size();

        memcpy(buffer, ss.c_str(), len + 1);
        return len;
    }
};

class FrameJpegReadCallback : public oatpp::data::stream::ReadCallback
{
   private:
    SyncApi& api;
    int old = -1;
    const FrameInfoController* self;
    int len = 0;
    std::string path;

   public:
    FrameJpegReadCallback(SyncApi& api, const FrameInfoController* self,
                          std::string p)
        : api(api), old(api.fc), self(self), path(p)
    {
    }

    char* ptr = nullptr;
    int size = -1;
    oatpp::v_io_size read(void* buffer, v_buff_size count,
                          oatpp::async::Action& action) override
    {
        // timer strategy adapted from oatpp/test/oatpp/web/app/ControllerAsync.hpp
        // check if the frame counter has changed
        if (api.fc == old) {
            // limits to 50fps; better would be to compute expected arrival time by storing framerate&lastTs
            action = oatpp::async::Action::createWaitRepeatAction(
                20 * 1000 + oatpp::Environment::getMicroTickCount());
            OATPP_LOGd("CAMH", "tick");
            return 0;
        }

        if (ptr == nullptr) {
            const std::string* ss = nullptr;

            if (path == "depth") {
                ss = api.zJpeg;
            } else if (path == "ampl") {
                ss = api.yJpeg;
            } else {
                std::cout << "Invalid jpeg path" << std::endl;
                return 0;
            }

            if (ss != nullptr) {
                size = ss->size();
                ptr = const_cast<char*>(ss->c_str());
            }
        }

        if (size > count) {
            memcpy(buffer, ptr, count);
            ptr = ptr + count;
            size -= count;
            return count;
        } else if (size > 0) {
            memcpy(buffer, ptr, count);
            ptr = ptr + count;
            size -= count;
            return count;
        } else {
            action = oatpp::async::Action::createActionByType(
                oatpp::async::Action::TYPE_NONE);
            return 0;
        }

        OATPP_LOGd("CAMH", "ticked {} dlen={}", api.fc, len);
        if (len) {
            // second iteration with new contents - tell the body-builder we are done:
            action = oatpp::async::Action::createActionByType(
                oatpp::async::Action::TYPE_NONE);
            return 0;
        }
        // first iteration with new contents - new data available, let's prepare an answer

        const std::string& ss = *api.zJpeg;
        len = ss.size();

        OATPP_LOGd("CAMH", "ticked {} dlen={} count={}", api.fc, len, count);

        memcpy(buffer, ss.c_str(), len + 1);
        return len;
    }
};

#include OATPP_CODEGEN_END(ApiController)  /// <-- End Code-Gen
