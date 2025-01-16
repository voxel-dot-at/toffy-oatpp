#pragma once

#include <string>

#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/utils/Conversion.hpp"

#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "toffy_oatpp/webapi/dto/frameDto.hpp"

#include "toffy_oatpp/shared/syncApi.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)  /// <-- Begin Code-Gen

class FrameInfoReadCallback;
class FrameMetaReadCallback;
class FrameJpegReadCallback;

static bool debugSync = false;

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
    ENDPOINT_ASYNC("GET", "/api/frame/info", GetFrameInfo)
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
    ENDPOINT_ASYNC("GET", "/api/frame/meta", GetFrameMeta)
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

    ENDPOINT_INFO(GetDepthMinMaxVal)
    {
        info->summary = "Get Min and Max Value for Image Processing";
        info->description = "Allows setting a custom min and max value for processing the frame image.";
        info->addResponse<Object<MinMaxDto>>(Status::CODE_200, "application/json");
    }
    
    ENDPOINT_ASYNC("GET", "/api/frame/depthMinMaxVal", GetDepthMinMaxVal)
    {
        ENDPOINT_ASYNC_INIT(GetDepthMinMaxVal)

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            auto dto = MinMaxDto::createShared();

            dto->minVal = controller->api->depthSettings.minVal;
            dto->maxVal = controller->api->depthSettings.maxVal;

            return _return(controller->createResponse(
                Status::CODE_200, controller->om->writeToString(dto)
            ));
        }
    };

    ENDPOINT_INFO(GetAmplMinMaxVal)
    {
        info->summary = "Get Min and Max Value for Image Processing";
        info->description = "Allows setting a custom min and max value for processing the frame image.";
        info->addResponse<Object<MinMaxDto>>(Status::CODE_200, "application/json");
    }
    
    ENDPOINT_ASYNC("GET", "/api/frame/amplMinMaxVal", GetAmplMinMaxVal)
    {
        ENDPOINT_ASYNC_INIT(GetAmplMinMaxVal)

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            auto dto = MinMaxDto::createShared();

            dto->minVal = controller->api->amplSettings.minVal;
            dto->maxVal = controller->api->amplSettings.maxVal;

            return _return(controller->createResponse(
                Status::CODE_200, controller->om->writeToString(dto)
            ));
        }
    };

    ENDPOINT_INFO(GetDepthMinVal)
    {
        info->summary = "Get Min Value for Image Processing";
        info->description = "Allows setting a custom min value for processing the frame image.";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }
    
    ENDPOINT_ASYNC("GET", "/api/frame/depthMinVal", GetDepthMinVal)
    {
        ENDPOINT_ASYNC_INIT(GetDepthMinVal)

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            double value = controller->api->depthSettings.minVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(GetAmplMinVal)
    {
        info->summary = "Get Min Value for Image Processing";
        info->description = "Allows setting a custom min value for processing the frame image.";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }
    
    ENDPOINT_ASYNC("GET", "/api/frame/amplMinVal", GetAmplMinVal)
    {
        ENDPOINT_ASYNC_INIT(GetAmplMinVal)

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            double value = controller->api->amplSettings.minVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(SetDepthMinVal)
    {
        info->summary = "Set Min Value for Image Processing";
        info->description =
            "Allows setting a custom min value for processing the frame image.";
        info->pathParams.add<String>("minVal").description =
            "The custom min value for image processing.";
    }

    ENDPOINT_ASYNC("POST", "/api/frame/depthMinVal/{minVal}", SetDepthMinVal)
    {
        ENDPOINT_ASYNC_INIT(SetDepthMinVal);

        Action act() override
        {
            auto minValStr = request->getPathVariable("minVal");

            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            double minVal = std::stod(minValStr->c_str());
            controller->api->depthSettings.ui_minVal = minVal;

            double value = controller->api->depthSettings.ui_minVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(SetAmplMinVal)
    {
        info->summary = "Set Min Value for Image Processing";
        info->description =
            "Allows setting a custom min value for processing the frame image.";
        info->pathParams.add<String>("minVal").description =
            "The custom min value for image processing.";
    }

    ENDPOINT_ASYNC("POST", "/api/frame/amplMinVal/{minVal}", SetAmplMinVal)
    {
        ENDPOINT_ASYNC_INIT(SetAmplMinVal);

        Action act() override
        {
            auto minValStr = request->getPathVariable("minVal");

            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            double minVal = std::stod(minValStr->c_str());
            controller->api->amplSettings.ui_minVal = minVal;

            double value = controller->api->amplSettings.ui_minVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(GetDepthMaxVal)
    {
        info->summary = "Get Max Value for Image Processing";
        info->description = "Allows setting a custom max value for processing the frame image.";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }

    ENDPOINT_ASYNC("GET", "/api/frame/depthMaxVal", GetDepthMaxVal)
    {
        ENDPOINT_ASYNC_INIT(GetDepthMaxVal)

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            double value = controller->api->depthSettings.maxVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(GetAmplMaxVal)
    {
        info->summary = "Get Max Value for Image Processing";
        info->description = "Allows setting a custom max value for processing the frame image.";
        info->addResponse<Int32>(Status::CODE_200, "application/json");
    }

    ENDPOINT_ASYNC("GET", "/api/frame/amplMaxVal", GetAmplMaxVal)
    {
        ENDPOINT_ASYNC_INIT(GetAmplMaxVal)

        Action act() override
        {
            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }

            double value = controller->api->amplSettings.maxVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(SetDepthMaxVal)
    {
        info->summary = "Set Max Value for Image Processing";
        info->description =
            "Allows setting a custom max value for processing the frame image.";
        info->pathParams.add<String>("maxVal").description =
            "The custom max value for image processing.";
    }

    ENDPOINT_ASYNC("POST", "/api/frame/depthMaxVal/{maxVal}", SetDepthMaxVal)
    {
        ENDPOINT_ASYNC_INIT(SetDepthMaxVal);

        Action act() override
        {
            auto maxValStr = request->getPathVariable("maxVal");

            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }
            double maxVal = std::stod(maxValStr->c_str());
            controller->api->depthSettings.ui_maxVal = maxVal;

            double value = controller->api->depthSettings.ui_maxVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(SetAmplMaxVal)
    {
        info->summary = "Set Max Value for Image Processing";
        info->description =
            "Allows setting a custom max value for processing the frame image.";
        info->pathParams.add<String>("maxVal").description =
            "The custom max value for image processing.";
    }

    ENDPOINT_ASYNC("POST", "/api/frame/amplMaxVal/{maxVal}", SetAmplMaxVal)
    {
        ENDPOINT_ASYNC_INIT(SetAmplMaxVal);

        Action act() override
        {
            auto maxValStr = request->getPathVariable("maxVal");

            if (!controller->api) {
                return _return(controller->createResponse(Status::CODE_404));
            }
            double maxVal = std::stod(maxValStr->c_str());
            controller->api->amplSettings.ui_maxVal = maxVal;

            double value = controller->api->amplSettings.ui_maxVal;

            return _return(controller->createResponse(
                Status::CODE_200, oatpp::String(std::to_string(value))));
        }
    };

    ENDPOINT_INFO(GetFrameDepth)
    {
        info->summary = "Get Frame JPEG";
        info->description = "Get the next frame as a JPEG image";
        info->pathParams.add<String>("path").description =
            "To get `depth` or `ampl` image, type either";
    }

    ENDPOINT_ASYNC("GET", "/api/frame/{path}", GetFrameDepth)
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
            if (debugSync) OATPP_LOGd("CAMH", "tick");
            return 0;
        }
        if (debugSync) OATPP_LOGd("CAMH", "ticked {} dlen={}", api.fc, len);
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
        if (debugSync) OATPP_LOGd("CAMM", "ticked {} dlen={}", api.fc, len);
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
        for (auto iter = slots.begin(); iter != slots.end(); iter++) {
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

    std::shared_ptr<std::string> shared;
    const char* ptr = nullptr;
    int size = -1;
    oatpp::v_io_size read(void* buffer, v_buff_size count,
                          oatpp::async::Action& action) override
    {
        // timer strategy adapted from oatpp/test/oatpp/web/app/ControllerAsync.hpp

        // check if the frame counter has changed, register interest
        if (api.fc == old) {
            // enable compressor
            if (path == "depth") { 
                api.wantDepth = true;
            } else if (path == "ampl") {
                api.wantAmpl = true;
            }

            // limits to 50fps; better would be to compute expected arrival time by storing framerate&lastTs
            action = oatpp::async::Action::createWaitRepeatAction(
                20 * 1000 + oatpp::Environment::getMicroTickCount());
            if (debugSync) OATPP_LOGd("CAMJ", "{} tick", path);
            return 0;
        }
        // new framecounter - first iteration: get the data to send:
        if (ptr == nullptr) {
            if (path == "depth") {
                shared = api.depthJpeg;
                ptr = shared->c_str();
            } else if (path == "ampl") {
                shared = api.amplJpeg;
                ptr = shared->c_str();
            } else {
                OATPP_LOGw("CAMJ", "Invalid jpeg path: {}", path);
                return 0;
            }
            if (ptr != nullptr) {
                size = shared->size();
            } else {
                OATPP_LOGw("CAMJ", "NO DATA for {}", path);
                return 0;
            }
        }
        // we have data, let's send it:
        if (size > count) { // send chunks
            memcpy(buffer, ptr, count);
            ptr = ptr + count;
            size -= count;
            return count;
        } else if (size > 0) { // last chunk
            int s = size;
            memcpy(buffer, ptr, size);
            size = 0;
            return s;
        } else { // end transfer
            shared.reset();
            ptr = nullptr;
            action = oatpp::async::Action::createActionByType(
                oatpp::async::Action::TYPE_NONE);
            return 0;
        }
    }
};

#include OATPP_CODEGEN_END(ApiController)  /// <-- End Code-Gen
