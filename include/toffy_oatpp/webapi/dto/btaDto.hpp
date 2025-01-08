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

    DTO_FIELD_INFO(connected)
    {
        info->description = "Indicates whether the camera is currently connected";
    }
    DTO_FIELD(Boolean, connected);

    DTO_FIELD_INFO(integrationTime)
    {
        info->description = "The integration time in microseconds (µs)";
    }
    DTO_FIELD(UInt32, integrationTime);

    DTO_FIELD_INFO(modulationFrequency)
    {
        info->description = "The modulation frequency in hertz (Hz)";
    }
    DTO_FIELD(UInt32, modulationFrequency);

    DTO_FIELD_INFO(frameRate)
    {
        info->description = "The current frame rate in frames per second (fps)";
    }
    DTO_FIELD(Float32, frameRate);

    DTO_FIELD_INFO(globalOffset)
    {
        info->description = "Provides the additive offset values (DistOffset0, DistOffset1, DistOffset2, DistOffset3) in hexadecimal format";
    }
    DTO_FIELD(Float32, globalOffset);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */