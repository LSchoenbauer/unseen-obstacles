#include <MoverDriverCfg.h>

MoverDriverCfg::MoverDriverCfg(
    uint8_t pulsePin,
    uint8_t dirPin,
    uint8_t topSwitchPin,
    uint8_t centerSwitchPin,
    uint8_t bottomSwitchPin,
    uint32_t rampingSteps,
    uint32_t microstepFactor
) :
    mPulsePin(pulsePin),
    mDirPin(dirPin),
    mTopSwitchPin(topSwitchPin),
    mCenterSwitchPin(centerSwitchPin),
    mBottomSwitchPin(bottomSwitchPin),
    mRampingSteps(rampingSteps),
    mMicrostepFactor(microstepFactor)
{}

MoverDriverCfgPtr MoverDriverCfg::Create(
  uint8_t pulsePin,
  uint8_t dirPin,
  uint8_t startPin,
  uint8_t centerPin,
  uint8_t endPin
) {
	return ::std::shared_ptr<MoverDriverCfg>(new MoverDriverCfg(
    pulsePin,
    dirPin,
    startPin,
    centerPin,
    endPin
  ));
}