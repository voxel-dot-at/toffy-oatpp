#pragma once

#include "rooms/Lobby.hpp"
#include "utils/Statistics.hpp"

#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/utils/Conversion.hpp"

#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include <regex>

// based on https://github.com/lganzzzo/canchat/blob/master/server/src/controller/FileController.hpp

#include OATPP_CODEGEN_BEGIN(ApiController)  /// <-- Begin Code-Gen

class CameraController : public oatpp::web::server::api::ApiController
{
   private:
    typedef FrameController __ControllerType;

   private:
    OATPP_COMPONENT(std::shared_ptr<Lobby>, lobby);

   public:
    FrameController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
                                    objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    {
    }

   public:
    ENDPOINT_ASYNC("GET", "room/{roomId}/file/{fileId}", GetFile)
    {
        ENDPOINT_ASYNC_INIT(GetFile)

        class ReadCallback : public oatpp::data::stream::ReadCallback
        {
           private:
            std::shared_ptr<File::Subscriber> m_subscriber;

           private:
            OATPP_COMPONENT(std::shared_ptr<Statistics>, m_statistics);

           public:
            ReadCallback(const std::shared_ptr<File::Subscriber>& subscriber)
                : m_subscriber(subscriber)
            {
            }

            oatpp::v_io_size read(void* buffer, v_buff_size count,
                                  oatpp::async::Action& action) override
            {
                auto res = m_subscriber->readChunk(buffer, count, action);
                if (res > 0) {
                    m_statistics->FILE_SERVED_BYTES += (v_uint64)res;
                }
                return res;
            }
        };

        Action act() override
        {
            oatpp::String roomId = request->getPathVariable("roomId");

            bool success;
            v_int64 fileId = oatpp::utils::Conversion::strToInt64(
                request->getPathVariable("fileId"), success);
            OATPP_ASSERT_HTTP(success, Status::CODE_400, "Invalid fileId");

            auto room = controller->lobby->getRoom(roomId);
            OATPP_ASSERT_HTTP(room, Status::CODE_404, "Room not found");

            auto file = room->getFileById(fileId);
            OATPP_ASSERT_HTTP(file, Status::CODE_404, "File not found");

            auto body = std::make_shared<
                oatpp::web::protocol::http::outgoing::StreamingBody>(
                std::make_shared<ReadCallback>(file->subscribe()));

            auto response =
                OutgoingResponse::createShared(Status::CODE_200, body);
            response->putHeader(
                "content-disposition",
                "attachment; filename=\"" + file->getFileName() + "\"");

            return _return(response);
        }
    };
};

#include OATPP_CODEGEN_END(ApiController)  /// <-- End Code-Gen
