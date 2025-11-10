/*
    receive commands from vr
    interprate commands
    controls MoverDriver
*/

#include "Translator.h"
#include "CommandData.h"
#include "Arduino.h"

const uint32_t NORMAL_MOVEMENT_SPEED = 60;

Translator::Translator(
    MoverDriverPtr rearLeft,
    MoverDriverPtr rearRight,
    MoverDriverPtr front,
    MoverDriverPtr rotation
) : mRearLeft(rearLeft), mRearRight(rearRight), mFront(front), mRotation(rotation), mCommandModeEnabled(false) {

}

Translator::~Translator() {

}

void Translator::SetCommandModeEnabled(bool enabled) {
    mCommandModeEnabled = enabled;
}

void Translator::ApplyCommand(const CommandData& commandData) {
    if (mCommandModeEnabled) {
        MoverDriverPtr mvr = GetMoverDriver(commandData.GetMover());
        switch (commandData.GetCommand()) {
            case CommandData::Command::UP:
                mvr->SetDirection(MoverDriver::Direction::FORWARD);
                mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                break;
            case CommandData::Command::DOWN:
                mvr->SetDirection(MoverDriver::Direction::BACKWARD);
                mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                break;
            case CommandData::Command::TO_TOP:
                bool isAtTop = mvr->IsAtTop();
                mvr->SetDirection(MoverDriver::Direction::FORWARD);
                while (!isAtTop) {
                    mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                    delayMicroseconds(30000);
                    isAtTop = mvr->IsAtTop();
                }
                break;
            case CommandData::Command::TO_CENTER:
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
            case CommandData::Command::TO_BOTTOM:
                bool isAtBottom = mvr->IsAtBottom();
                mvr->SetDirection(MoverDriver::Direction::BACKWARD);
                while (!isAtBottom) {
                    mvr->SetSpeedRpm(NORMAL_MOVEMENT_SPEED);
                    delayMicroseconds(30000);
                    isAtTop = mvr->IsAtBottom();
                }
                break;
        }
    }
}

void Translator::AdjustToSimulation(const SimulationData& simulationData) {

}

MoverDriverPtr Translator::GetMoverDriver(CommandData::Mover mover) {
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

