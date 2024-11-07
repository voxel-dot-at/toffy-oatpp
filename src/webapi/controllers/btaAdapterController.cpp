#include "webapi/controller/btaAdapterController.hpp"

void BtaAdapterController::populateBtaSettings(BtaSettingsDTO& dto)
{
    BtaWrapper* s = bta->getSensor();

    dto.connected = s->getState() == BtaWrapper::ConnState::connected;
    dto.integrationTime = s->getIntegrationTime();
    dto.modulationFrequency = s->getModulationFrequency();
    dto.frameRate = s->getFrameRate();
    dto.globalOffset = s->getGlobalOffset();
}
