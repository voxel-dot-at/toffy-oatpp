/*
   Copyright 2012-2025 Simon Vogl <svogl@voxel.at> VoXel Interaction Design - www.voxel.at

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

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