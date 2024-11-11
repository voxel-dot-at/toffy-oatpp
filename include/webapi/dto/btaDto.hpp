#ifndef BTA_DTO_HPP
#define BTA_DTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class BtaSettingsDTO : public oatpp::DTO
{
    DTO_INIT(BtaSettingsDTO, DTO);

    DTO_FIELD(Boolean, connected);

    DTO_FIELD(UInt32, integrationTime);
    DTO_FIELD(UInt32, modulationFrequency);
    DTO_FIELD(Float32, frameRate);
    DTO_FIELD(Float32, globalOffset);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */