#ifndef WEB_ADAP_DTO_HPP
#define WEB_ADAP_DTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include "measurement.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class CameraSettings : public oatpp::DTO {
  
  DTO_INIT(CameraSettings, DTO)
  
  DTO_FIELD(Int32, fc);

  DTO_FIELD(Int32, fc1);

};


#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */