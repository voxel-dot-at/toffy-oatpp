#ifndef WEB_ADAP_DTO_HPP
#define WEB_ADAP_DTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"


#include OATPP_CODEGEN_BEGIN(DTO)

class SlotInfoDto : public oatpp::DTO {

  DTO_INIT(SlotInfoDto, DTO /* extends */)

  DTO_FIELD(String, name);
  DTO_FIELD(Int32, dt);
  DTO_FIELD(String, desc);
};

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class FrameDto : public oatpp::DTO {
  
  DTO_INIT(FrameDto, DTO)

  DTO_FIELD(Int32, fc);

  DTO_FIELD(Int32, ts);

  DTO_FIELD(Int32, mainTemp);

  DTO_FIELD(Int32, ledTemp);

  DTO_FIELD(Vector<Object<SlotInfoDto>>, slots) = {};

  DTO_FIELD(Vector<Object<SlotInfoDto>>, mats) = {};

  DTO_FIELD(Vector<String>, fields);

  DTO_FIELD(Vector<Int16>, dts);

  DTO_FIELD(Vector<String>, desc);
};


#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */