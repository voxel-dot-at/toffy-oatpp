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

#include "toffy_oatpp/webapi/controller/btaAdapterController.hpp"

void BtaAdapterController::populateBtaSettings(BtaSettingsDTO& dto)
{
    BtaWrapper* s = bta->getSensor();

    dto.connected = s->getState() == BtaWrapper::ConnState::connected;
    dto.integrationTime = s->getIntegrationTime();
    dto.modulationFrequency = s->getModulationFrequency();
    dto.frameRate = s->getFrameRate();
    dto.globalOffset = s->getGlobalOffset();
}
