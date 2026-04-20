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
#include <os/TaskMgmt.h>

const uint32_t ChairController::NORMAL_MOVEMENT_SPEED = 300; //120 //60
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
    LogDbg("ChairController: Command mode set");
}

void ChairController::ApplyCommand(const CommandData& commandData) {
    //TODO: Just for debugging - remove
    MoverDriverPtr mvr = GetMoverDriver(commandData.GetMover());
    //LogDbg("Is at top: %d",mvr->IsAtTop());
    //LogDbg("Is at center: %d", mvr->IsAtCenter());
    //LogDbg("Is at bottom: %d", mvr->IsAtBottom());
    // end of dbg code

    if (xSemaphoreTake(mCtrlMutex, portMAX_DELAY) == pdTRUE) {
        LogDbg("ChairController: Applying command [%d] for mover [%d]", (int)commandData.GetCommand(), (int)commandData.GetMover());
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

void ChairController::CmdUp(MoverDriverPtr mvr) {
    LogDbg("Moving up");

    for (int i = 0; i < 70; ++i) {
        mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED,
                                 MoverDriver::Direction::FORWARD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ChairController::CmdDown(MoverDriverPtr mvr) {
    for (int i = 0; i < 70; ++i) {
        mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED,
                             MoverDriver::Direction::BACKWARD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ChairController::CmdToTop(MoverDriverPtr mvr) {
    while (!mvr->IsAtTop()) {
        mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED,
                                 MoverDriver::Direction::FORWARD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ChairController::CmdToCenter(MoverDriverPtr mvr)
{
    auto direction =
        mvr->GetCurrentPosition() < mvr->GetCenterPosition()
        ? MoverDriver::Direction::FORWARD
        : MoverDriver::Direction::BACKWARD;

    mvr->SetDirection(direction);

    while (!mvr->IsAtCenter()) {

        if (mvr->IsAtTop())
            direction = MoverDriver::Direction::BACKWARD;
        else if (mvr->IsAtBottom())
            direction = MoverDriver::Direction::FORWARD;

        mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED, direction);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ChairController::CmdToBottom(MoverDriverPtr mvr)
{
    while (!mvr->IsAtBottom()) {
        mvr->SetSpeedAndDirection(NORMAL_MOVEMENT_SPEED,
                                 MoverDriver::Direction::BACKWARD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ChairController::OnCommand(const CtrlCommandData& cmdData) {
    LogDbg("ChairController: Received command [%d] for mover [%d]", (int)cmdData.mCommand, (int)cmdData.mMover);
    SetCommandModeEnabled(true);
    MoverDriverPtr mvr = GetMoverDriver(cmdData.mMover);
    switch (cmdData.mCommand)
    {
        case CommandData::Command::UP:        CmdUp(mvr); break;
        case CommandData::Command::DOWN:      CmdDown(mvr); break;
        case CommandData::Command::TO_TOP:    CmdToTop(mvr); break;
        case CommandData::Command::TO_CENTER: CmdToCenter(mvr); break;
        case CommandData::Command::TO_BOTTOM: CmdToBottom(mvr); break;

        case CommandData::Command::CALIBRATE:
            CmdToCenter(mRearLeft);
            CmdToCenter(mRearRight);
            CmdToCenter(mFront);
            CmdToCenter(mRotation);
            break;

        case CommandData::Command::MOUNT:
            CmdToBottom(mRearLeft);
            CmdToBottom(mRearRight);
            CmdToBottom(mFront);
            CmdToBottom(mRotation);
            break;

        case CommandData::Command::SHUT_DOWN:
            CmdToBottom(mRearLeft);
            CmdToBottom(mRearRight);
            CmdToBottom(mFront);
            CmdToBottom(mRotation);
            break;

        case CommandData::Command::TRANSFER_CONTROL:
            SetCommandModeEnabled(false);
            break;
    }
}

void ChairController::OnSimulationData(const CtrlSimulationData& simData) {
    if (!mCommandModeEnabled) {
        // TODO Methoden Auslagerung
        if (mLastSimData.mTimestampMs == 0) {
            mLastSimData = simData;
        }

        const uint32_t MIN_TURN = 270;
        const uint32_t MAX_TURN = 90;

        int32_t deltaTimestamp = simData.mTimestampMs - mLastSimData.mTimestampMs;
        
        int32_t deltaYaw = (mLastSimData.mYaw > MIN_TURN && simData.mYaw < MAX_TURN) 
                            ? (simData.mYaw + 360) - mLastSimData.mYaw
                            : (simData.mYaw > MIN_TURN && mLastSimData.mYaw < MAX_TURN)
                                ? simData.mYaw - (mLastSimData.mYaw + 360)
                                : simData.mYaw - mLastSimData.mYaw;

        //int32_t deltaYaw = simData.mYaw - mLastSimData.mYaw;
        int32_t deltaPitch = simData.mPitch - mLastSimData.mPitch;
        int32_t deltaRoll = simData.mRoll - mLastSimData.mRoll;
        int32_t speedZ = simData.mPosZ - mLastSimData.mPosZ;
        int32_t accZ = speedZ - mLastSpeedZ;

        ApplyRotation(deltaTimestamp, deltaYaw);

        if (simData.mMode == SimulationData::Mode::BUMPING) {
            uint32_t wheelchairSpeed = sqrt(pow(simData.mPosX - mLastSimData.mPosX, 2) + pow(simData.mPosY - mLastSimData.mPosY, 2));
            ApplyShakeMode(deltaTimestamp, INTENSITY_SHAKING, wheelchairSpeed);
        } else {
            ApplyFrontMover(deltaTimestamp, deltaPitch, accZ);
            ApplyBackMover(deltaTimestamp, deltaRoll, accZ);
        }

        mLastSimData = simData;
        mLastSpeedZ = speedZ;
    }
}

void ChairController::ApplyRotation(uint32_t deltaTimestamp, int32_t deltaYaw) {
    double rotationLength = std::abs(M_PI * CIRCUMFERENCE_ROTATOR_WHEEL * deltaYaw / (180.0 * 100.0)); 
    double stepperTurns = rotationLength / (double)CIRCUMFERENCE_ROTATOR_WHEEL;

    uint32_t rotationRpm = round(stepperTurns * 60000 / deltaTimestamp);

    MoverDriverPtr rotator = GetMoverDriver(CommandData::Mover::ROTATION);
    rotator->SetSpeedRpm(rotationRpm);
    rotator->SetDirection(deltaYaw < 0 ? MoverDriver::Direction::BACKWARD : MoverDriver::Direction::FORWARD);
}

void ChairController::ApplyFrontMover(uint32_t deltaTimestamp, int32_t deltaPitch, int32_t deltaStroke) {
    double stroke = (std::sin(DegToRad(deltaPitch / 100.0)) * DISTANCE_FRONT) + (deltaStroke / 0.5);
    LogDbg("Sin calculation, front mover %f = %f %f %f", stroke, std::sin(deltaPitch / 100.0), (std::sin(deltaPitch / 100.0) * DISTANCE_FRONT), (deltaStroke / 100.0));
    double stepperTurns = std::abs(stroke / (double)STROKE_PER_TURN);

    uint32_t rotationRpm = round(stepperTurns * 60000 / deltaTimestamp);

    MoverDriverPtr frontMover = GetMoverDriver(CommandData::Mover::FRONT);
    frontMover->SetSpeedRpm(rotationRpm);
    frontMover->SetDirection(stroke < 0 ? MoverDriver::Direction::BACKWARD : MoverDriver::Direction::FORWARD);
}

double ChairController::DegToRad(double deg) {
    return deg * M_PI / 180.0;
}

void ChairController::ApplyBackMover(uint32_t deltaTimestamp, int32_t deltaRoll, int32_t deltaStroke) {
    double stroke = (std::sin(DegToRad(deltaRoll / 100.0)) * DISTANCE_SIDE_BACK) + (deltaStroke / 0.5);
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

