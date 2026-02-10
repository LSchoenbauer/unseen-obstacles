#include <MoverDriverCfg.h>

const uint32_t MoverDriverCfg::DEFAULT_RAMPING_STEPS = 50;
const uint32_t MoverDriverCfg::FULL_REVOLUTION_STEP_COUNT = 200;

MoverDriverCfg::MoverDriverCfg(
    const char* label,
    uint8_t pulsePin,
    uint8_t dirPin,
    uint8_t topSwitchPin,
    uint8_t centerSwitchPin,
    uint8_t bottomSwitchPin,
    uint32_t microstepFactor,
    uint32_t rampingSteps
) :
    mLabel(label),
    mPulsePin(pulsePin),
    mDirPin(dirPin),
    mTopSwitchPin(topSwitchPin),
    mCenterSwitchPin(centerSwitchPin),
    mBottomSwitchPin(bottomSwitchPin),
    mMicrostepFactor(microstepFactor),
    mRampingSteps(rampingSteps)
{
}

MoverDriverCfgPtr MoverDriverCfg::Create(
    const char* label,
    uint8_t pulsePin,
    uint8_t dirPin,
    uint8_t startPin,
    uint8_t centerPin,
    uint8_t endPin,
    uint32_t microstepFactor
) {
  return Create(
    label,
    pulsePin, dirPin,
    startPin, centerPin, endPin,
    microstepFactor, DEFAULT_RAMPING_STEPS
  );
}

MoverDriverCfgPtr MoverDriverCfg::Create(
    const char* label,
    uint8_t pulsePin,
    uint8_t dirPin,
    uint8_t startPin,
    uint8_t centerPin,
    uint8_t endPin,
    uint32_t microstepFactor,
    uint32_t rampingSteps
) {
  MoverDriverCfg* cfg = new MoverDriverCfg(
    label,
    pulsePin, dirPin,
    startPin, centerPin, endPin,
    microstepFactor, rampingSteps
  );
	return ::std::shared_ptr<MoverDriverCfg>(cfg, [](MoverDriverCfg* object){
    delete object;
  });
}