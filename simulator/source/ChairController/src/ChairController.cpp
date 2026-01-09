/*
    receive commands from vr
    interprate commands
    controls MoverDriver
*/

#include "ChairController.h"
#include "CommandData.h"
#include "Arduino.h"
#include "cmath"

const uint32_t ChairController::NORMAL_MOVEMENT_SPEED = 60;
// TODO echte Werte bemessen
const uint32_t ChairController::DISTANCE_ROTATOR = 600;
const uint32_t ChairController::CIRCUMFERENCE_ROTATOR_WHEEL = 314; 
const uint32_t ChairController::DISTANCE_FRONT = 450;
const uint32_t ChairController::STROKE_PER_TURN = 5;
const uint32_t ChairController::DISTANCE_SIDE_BACK = 250;

ChairController::ChairController(
    MoverDriverPtr rearLeft,
    MoverDriverPtr rearRight,
    MoverDriverPtr front,
    MoverDriverPtr rotation
) : mRearLeft(rearLeft), mRearRight(rearRight), mFront(front), mRotation(rotation), mCommandModeEnabled(false) {

}

ChairController::~ChairController() {

}

void ChairController::SetCommandModeEnabled(bool enabled) {
    mCommandModeEnabled = enabled;
}

void ChairController::ApplyCommand(const CommandData& commandData) {
    if (mCommandModeEnabled) {
        MoverDriverPtr mvr = GetMoverDriver(commandData.GetMover());
        switch (commandData.GetCommand()) {
            case CommandData::Command::UP: // TODO Methoden Auslagerung für alle cases
                mvr->SetDirection(MoverDriver::Direction::FORWARD);
                mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                break;
            case CommandData::Command::DOWN:
                mvr->SetDirection(MoverDriver::Direction::BACKWARD);
                mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                break;
            case CommandData::Command::TO_TOP:{
                bool isAtTop = mvr->IsAtTop();
                mvr->SetDirection(MoverDriver::Direction::FORWARD);
                while (!isAtTop) {
                    mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                    delayMicroseconds(30000);
                    isAtTop = mvr->IsAtTop();
                }
                break;
            }
            case CommandData::Command::TO_CENTER:{
                bool isAtTop = mvr->IsAtTop();
                bool isAtBottom = mvr->IsAtBottom();
                bool isAtCenter = mvr->IsAtCenter();
                MoverDriver::Direction direction = mvr->GetCurrentPosition() < mvr->GetCenterPosition() ? MoverDriver::Direction::FORWARD : MoverDriver::Direction::BACKWARD; 
                mvr->SetDirection(direction);
                while (!isAtCenter) {
                    if (isAtTop) {
                        direction = MoverDriver::Direction::BACKWARD;
                    } else if (isAtBottom) {
                        direction = MoverDriver::Direction::FORWARD;
                    }
                    mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                    delayMicroseconds(30000);
                    isAtTop = mvr->IsAtTop();
                    isAtBottom = mvr->IsAtBottom();
                    isAtCenter = mvr->IsAtCenter();
                }
                break;
            }
            case CommandData::Command::TO_BOTTOM:{
                bool isAtBottom = mvr->IsAtBottom();
                mvr->SetDirection(MoverDriver::Direction::BACKWARD);
                while (!isAtBottom) {
                    mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                    delayMicroseconds(30000);
                    isAtBottom = mvr->IsAtBottom();
                }
                break;
            }
        }
    }
}

void ChairController::AdjustToSimulation(const SimulationData& simulationData) {
    // TODO Methoden Auslagerung
    static SimulationData lastData = simulationData;
    static uint32_t lastSpeedZ = 0;
    uint32_t deltaTimestamp = simulationData.GetTimestampMs() - lastData.GetTimestampMs();
    uint32_t deltaYaw = simulationData.GetYaw() - lastData.GetYaw();
    uint32_t deltaPitch = simulationData.GetPitch() - lastData.GetPitch();
    uint32_t deltaRoll = simulationData.GetRoll() - lastData.GetRoll();
    uint32_t speedZ = simulationData.GetPosZ() - lastData.GetPosZ();
    uint32_t accZ = speedZ - lastSpeedZ;

    ApplyRotation(deltaTimestamp, deltaYaw);
    ApplyFrontMover(deltaTimestamp, deltaPitch, accZ);
    ApplyBackMover(deltaTimestamp, deltaRoll, accZ); // TODO method call signature check
    if (simulationData.GetMode() == SimulationData::Mode::BUMPING) {
        ApplyShakeMode(deltaTimestamp);
    }

    lastData = simulationData;
    lastSpeedZ = speedZ;
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

void ChairController::ApplyShakeMode(uint32_t deltaTimestamp) {
    uint32_t randomRoll = rand() % 301;  // 0–300
    uint32_t randomDirection = rand() % 2;
    randomRoll = randomDirection % 2 == 0 ? randomRoll : 1 / randomRoll;
    uint32_t randomSpeedMs = rand() % 300 + 50;

    ApplyBackMover(deltaTimestamp, randomRoll, randomSpeedMs);
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

