#include "SimulationData.h"
#include <utils/Log.h>


const uint8_t SimulationData::POOL_SIZE = 8;
SimulationData SimulationData::mPool[POOL_SIZE];
SemaphoreHandle_t SimulationData::mPoolSemaphore = xSemaphoreCreateCounting(POOL_SIZE, POOL_SIZE);

SimulationData::SimulationData() : mTimestampMs(0),
                                   mMode(Mode::NORMAL),
                                   mPitch(0),
                                   mYaw(0),
                                   mRoll(0),
                                   mPosX(0),
                                   mPosY(0),
                                   mPosZ(0)
{
}

SimulationData::~SimulationData() {
}

SimulationData* SimulationData::Acquire(
    uint32_t timestampMs,
    SimulationData::Mode mode,
    uint32_t pitch,
    uint32_t yaw,
    uint32_t roll,
    uint32_t posX,
    uint32_t posY,
    uint32_t posZ) 
{
  SimulationData* data = nullptr;
  // wait for a free instance
  if (xSemaphoreTake(mPoolSemaphore, portMAX_DELAY) == pdTRUE) {
    uint8_t i = 0;
    for (; i < POOL_SIZE && mPool[i].GetTimestampMs() != 0; i++);
    if (i < POOL_SIZE) {
      data = &mPool[i];
    }
    if (data != nullptr) {
        data->mTimestampMs = timestampMs == 0 ? 1 : timestampMs; // avoid 0 timestamp
        data->mMode = mode;
        data->mPitch = pitch;
        data->mYaw = yaw;
        data->mRoll = roll;
        data->mPosX = posX;
        data->mPosY = posY;
        data->mPosZ = posZ;
      }
    }
    return data;
}

void SimulationData::Release() {
  mTimestampMs = 0;
  xSemaphoreGive(SimulationData::mPoolSemaphore);
}

