#include "CommandData.h"
#include <utils/Log.h>


const uint8_t CommandData::POOL_SIZE = 8;
CommandData CommandData::mPool[POOL_SIZE];
SemaphoreHandle_t CommandData::mPoolSemaphore = xSemaphoreCreateCounting(POOL_SIZE, POOL_SIZE);

CommandData::CommandData() : mAllocated(false),
                             mCommand(Command::UP),
                             mMover(Mover::REAR_LEFT)
{
}

CommandData::~CommandData() {
}

CommandData* CommandData::Acquire(
    Command command,
    Mover mover)
{
  CommandData* data = nullptr;
  // wait for a free instance
  if (xSemaphoreTake(mPoolSemaphore, portMAX_DELAY) == pdTRUE) {
    uint8_t i = 0;
    for (; i < POOL_SIZE && mPool[i].mAllocated; i++);
    if (i < POOL_SIZE) {
      data = &mPool[i];
    }
    if (data != nullptr) {
      data->mAllocated = true;
      data->mCommand = command;
      data->mMover = mover;
    }
    return data;
  }
}

void CommandData::Release() {
  mAllocated = false;
  xSemaphoreGive(CommandData::mPoolSemaphore);
}

