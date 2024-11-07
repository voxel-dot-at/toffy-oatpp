#ifndef WEB_CONTROLLER_HPP
#define WEB_CONTROLLER_HPP

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/base/Log.hpp"

//#include "globals.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)  //<-- Begin Codegen

/**
 * controller for static web contents.
 */
class StaticContentsController : public oatpp::web::server::api::ApiController
{
   public:
    /**
   * Constructor with object mapper.
   * @param apiContentMappers - mappers used to serialize/deserialize `s.
   */
    StaticContentsController(OATPP_COMPONENT(
        std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
    }

    static std::string getContentType(const std::string& path)
    {
        const size_t i = path.find_last_of(".");
        if (i != std::string::npos) {
            const std::string extension = path.substr(i + 1);
            if (extension == "html" || extension == "htm" ||
                extension == "shtml")
                return "text/html";
            if (extension == "js") return "application/javascript";
            if (extension == "css") return "text/css";
            if (extension == "jpg" || extension == "jpg") return "image/jpeg";
            if (extension == "gif") return "image/gif";
            if (extension == "ico") return "image/x-icon";
            if (extension == "png") return "image/png";
            if (extension == "svg" || extension == "svgz")
                return "image/svg+xml";
        }
        return "";  // if the path has no "." or has unknow extension we can not find the correct mime as well.
    };

   public:
    /*
    ENDPOINT("GET", "/", root)
    {
        auto dto = TestDto::createShared();
        dto->statusCode = 200;
        dto->topic = "funky/topic";
        dto->message = "Hello World!";
        return createDtoResponse(Status::CODE_200, dto);
    }
    */
    ENDPOINT("GET", "*", wildcard,
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        {
            std::string path = request->getPathTail();
            // Load the home page in case of no path, it means calling root like: http://localhost/
            if (path.empty()) {
                path =
                    "index.html";  // This is my default home file, you can set your own home page as well.
            }
            if (path.at(path.size() - 1) == '/') {
                path += "index.html";
            }
            // We will check the file if exist and send the index.html in case of not
            OATPP_LOGd("loading {}", ("webapi/" + path));
            auto file = oatpp::String::loadFromFile(("webapi/" + path).c_str());
            // Send 404 not found in case of no file
            OATPP_ASSERT_HTTP(file.get() != nullptr, Status::CODE_404,
                              "File not found");

            // As file already found, we can search the content type
            const std::string contentType = getContentType(path);

            // Creating the response
            auto response = createResponse(Status::CODE_200, file);
            if (contentType
                    .size())  // Add the content-type header only if we have a known mime
                response->putHeader(Header::CONTENT_TYPE, contentType);
            return response;
        };
    };
};

#include OATPP_CODEGEN_END(ApiController)  //<-- End Codegen

#endif /* MyController_hpp */