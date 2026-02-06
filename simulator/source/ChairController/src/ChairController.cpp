/*
    receive commands from vr
    interprate commands
    controls MoverDriver
*/

#include "ChairController.h"
#include "CommandData.h"
#include "Arduino.h"
#include "cmath"
#include <utils/Log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

const uint32_t ChairController::NORMAL_MOVEMENT_SPEED = 60;
// TODO echte Werte bemessen
const uint32_t ChairController::DISTANCE_ROTATOR = 600;
const uint32_t ChairController::CIRCUMFERENCE_ROTATOR_WHEEL = 314; 
const uint32_t ChairController::DISTANCE_FRONT = 450;
const uint32_t ChairController::STROKE_PER_TURN = 5;
const uint32_t ChairController::DISTANCE_SIDE_BACK = 250;
const uint32_t ChairController::INTENSITY_SHAKING = 10;

ChairController::ChairController(
    MoverDriverPtr rearLeft,
    MoverDriverPtr rearRight,
    MoverDriverPtr front,
    MoverDriverPtr rotation) : 
        mCtrlQueue(nullptr), mCtrlMutex(nullptr),
        mRearLeft(rearLeft), mRearRight(rearRight), 
        mFront(front), mRotation(rotation), 
        mCommandModeEnabled(false),
        mLastSimData(),
        mLastSpeedZ(0)
{
        mCtrlMutex = xSemaphoreCreateMutex();
        mCtrlQueue = xQueueCreate(1, sizeof(CtrlData));
}

ChairController::~ChairController() {
}

ChairControllerPtr ChairController::Create(
    MoverDriverPtr rearLeft,
    MoverDriverPtr rearRight,
    MoverDriverPtr front,
    MoverDriverPtr rotation) {
	return ::std::shared_ptr<ChairController>(new ChairController(
        rearLeft,
        rearRight,
        front,
        rotation
    ));
}

// TODO: Probably better solution
//       Switch automatically to command mode when a command is received
//       Reduce this method to returning to simulation mode "DisableCommandMode"
void ChairController::SetCommandModeEnabled(bool enabled) {
    mCommandModeEnabled = enabled;
    LogDbg("Command mode set");
}

void ChairController::ApplyCommand(const CommandData& commandData) {
    if (xSemaphoreTake(mCtrlMutex, portMAX_DELAY) == pdTRUE) {
        CtrlCommandData data(commandData);
        CtrlData ctrlData(data);
        // overwrite the queue with the latest command, older commands are out of interest
        xQueueOverwrite(mCtrlQueue, &ctrlData); 
        xSemaphoreGive(mCtrlMutex);
    }
}

void ChairController::AdjustToSimulation(const SimulationData& simulationData) {
    if (xSemaphoreTake(mCtrlMutex, portMAX_DELAY) == pdTRUE) {
        if (!mCommandModeEnabled) {
            CtrlSimulationData data(simulationData);
            CtrlData ctrlData(data);
            // overwrite the queue with the latest command, older commands are out of interest
            xQueueOverwrite(mCtrlQueue, &ctrlData); 
        }
        xSemaphoreGive(mCtrlMutex);
    }
}

void ChairController::OnEvent(AppEventPtr ev) {
    CtrlData ctrlData;
    xQueueReceive(mCtrlQueue, &ctrlData, portMAX_DELAY); 
    switch (ctrlData.mMode) {
        case CtrlMode::COMMAND:
            OnCommand(ctrlData.mCommandData);
            break;
        case CtrlMode::SIMULATION:
            OnSimulationData(ctrlData.mSimulationData);
            break;
        case CtrlMode::NONE:
            // do nothing
            break;
    }
}

void ChairController::OnCommand(const CtrlCommandData& cmdData) {
    SetCommandModeEnabled(true);
    MoverDriverPtr mvr = GetMoverDriver(cmdData.mMover);
    switch (cmdData.mCommand) {
        case CommandData::Command::UP: // TODO Methoden Auslagerung für alle cases
            LogDbg("Moving up");
            for (int i = 0; i < 30; i++) {
                mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED, MoverDriver::Direction::FORWARD);
                vTaskDelay(pdMS_TO_TICKS(33));
            }
            break;
        case CommandData::Command::DOWN:
            mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED, MoverDriver::Direction::BACKWARD);
            break;
        case CommandData::Command::TO_TOP:{
            while (!mvr->IsAtTop()) {
                mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED, MoverDriver::Direction::FORWARD);
                vTaskDelay(pdMS_TO_TICKS(30));
            }
            break;
        }
        case CommandData::Command::TO_CENTER:{
            MoverDriver::Direction direction = mvr->GetCurrentPosition() < mvr->GetCenterPosition() ? MoverDriver::Direction::FORWARD : MoverDriver::Direction::BACKWARD; 
            mvr->SetDirection(direction);
            while (!mvr->IsAtCenter()) {
                if (mvr->IsAtTop()) {
                    direction = MoverDriver::Direction::BACKWARD;
                } else if (mvr->IsAtBottom()) {
                    direction = MoverDriver::Direction::FORWARD;
                }
                mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED, direction);
                vTaskDelay(pdMS_TO_TICKS(30));
            }
            break;
        }
        case CommandData::Command::TO_BOTTOM:{
            while (!mvr->IsAtBottom()) {
                mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED, MoverDriver::Direction::BACKWARD);
                vTaskDelay(pdMS_TO_TICKS(30));
            }
            break;
        }
    }
}

void ChairController::OnSimulationData(const CtrlSimulationData& simData) {
    if (!mCommandModeEnabled) {
        // TODO Methoden Auslagerung
        if (mLastSimData.mTimestampMs == 0) {
            mLastSimData = simData;
        }

        uint32_t deltaTimestamp = simData.mTimestampMs - mLastSimData.mTimestampMs;
        uint32_t deltaYaw = simData.mYaw - mLastSimData.mYaw;
        uint32_t deltaPitch = simData.mPitch - mLastSimData.mPitch;
        uint32_t deltaRoll = simData.mRoll - mLastSimData.mRoll;
        uint32_t speedZ = simData.mPosZ - mLastSimData.mPosZ;
        uint32_t accZ = speedZ - mLastSpeedZ;

        ApplyRotation(deltaTimestamp, deltaYaw);

        if (simData.mMode == SimulationData::Mode::BUMPING) {
            uint32_t wheelchairSpeed = sqrt(pow(simData.mPosX - mLastSimData.mPosX, 2) + pow(simData.mPosY - mLastSimData.mPosY, 2));
            ApplyShakeMode(deltaTimestamp, INTENSITY_SHAKING, wheelchairSpeed);
        } else {
            ApplyFrontMover(deltaTimestamp, deltaPitch, accZ);
            ApplyBackMover(deltaTimestamp, deltaRoll, accZ); // TODO method call signature check
        }

        mLastSimData = simData;
        mLastSpeedZ = speedZ;
    }
}

void ChairController::ApplyRotation(uint32_t deltaTimestamp, uint32_t deltaYaw) {
    double rotationLength = std::abs(M_PI * CIRCUMFERENCE_ROTATOR_WHEEL * deltaYaw / 180.0);
    double stepperTurns = rotationLength / (double)CIRCUMFERENCE_ROTATOR_WHEEL;

    uint32_t rotationRpm = round(stepperTurns * 60000 / deltaTimestamp);

    MoverDriverPtr rotator = GetMoverDriver(CommandData::Mover::ROTATION);
    rotator->SetSpeedRpm(rotationRpm);
    rotator->SetDirection(deltaYaw < 0 ? MoverDriver::Direction::BACKWARD : MoverDriver::Direction::FORWARD);
}

void ChairController::ApplyFrontMover(uint32_t deltaTimestamp, uint32_t deltaPitch, uint32_t deltaStroke) {
    double stroke = (std::sin(deltaPitch) * DISTANCE_FRONT) + deltaStroke;
    double stepperTurns = std::abs(stroke / (double)STROKE_PER_TURN);

    uint32_t rotationRpm = round(stepperTurns * 60000 / deltaTimestamp);

    MoverDriverPtr frontMover = GetMoverDriver(CommandData::Mover::FRONT);
    frontMover->SetSpeedRpm(rotationRpm);
    frontMover->SetDirection(stroke < 0 ? MoverDriver::Direction::BACKWARD : MoverDriver::Direction::FORWARD);
}

void ChairController::ApplyBackMover(uint32_t deltaTimestamp, uint32_t deltaRoll, uint32_t deltaStroke) {
    double stroke = (std::sin(deltaRoll) * DISTANCE_SIDE_BACK) + deltaStroke;
    double stepperTurns = std::abs(stroke / (double)STROKE_PER_TURN);

    uint32_t rotationRpm = round(stepperTurns * 60000 / deltaTimestamp);

    MoverDriverPtr rearLeftMover = GetMoverDriver(CommandData::Mover::REAR_LEFT);
    MoverDriverPtr rearRightMover = GetMoverDriver(CommandData::Mover::REAR_RIGHT);
    rearLeftMover->SetSpeedRpm(rotationRpm);
    rearLeftMover->SetDirection(stroke < 0 ? MoverDriver::Direction::BACKWARD : MoverDriver::Direction::FORWARD);
    rearRightMover->SetSpeedRpm(rotationRpm);
    rearRightMover->SetDirection(stroke < 0 ? MoverDriver::Direction::FORWARD : MoverDriver::Direction::BACKWARD);
}

void ChairController::ApplyShakeMode(uint32_t deltaTimestamp, uint32_t intensity, uint32_t wheelchairSpeed) {
    //TODO wheelchair speed
    static uint32_t duration = 601;
    if (duration > 600) {
        uint32_t randomRoll = (355 + rand() % intensity) % 360;
        uint32_t randomPitch = (355 + rand() % intensity) % 360;

        ApplyBackMover(deltaTimestamp, randomRoll, 0);
        ApplyFrontMover(deltaTimestamp, randomPitch, 0);
        duration = 0;
    } else {
        duration += deltaTimestamp;
    }
}

MoverDriverPtr ChairController::GetMoverDriver(CommandData::Mover mover) {
    switch (mover) {
        case CommandData::Mover::REAR_LEFT:
        return mRearLeft;
        case CommandData::Mover::REAR_RIGHT:
        return mRearRight;
        case CommandData::Mover::FRONT:
        return mFront;
        case CommandData::Mover::ROTATION:
        return mRotation;
    }
    return nullptr;
}

