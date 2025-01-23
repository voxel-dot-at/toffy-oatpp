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

#pragma once

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

  DTO_FIELD(Int32, mainTemp);

  DTO_FIELD(Int32, ledTemp);
};

class FrameMetaDto : public oatpp::DTO {

  DTO_INIT(FrameMetaDto, DTO)

  DTO_FIELD(Vector<Object<SlotInfoDto>>, slots) = {};

  DTO_FIELD(Vector<Object<SlotInfoDto>>, mats) = {};

  DTO_FIELD(Vector<String>, fields);
};

class MinMaxDto : public oatpp::DTO {
  DTO_INIT(MinMaxDto, DTO)

  DTO_FIELD(Float32, minVal);
  
  DTO_FIELD(Float32, maxVal);
};