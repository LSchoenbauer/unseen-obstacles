#include "ChairControllerApp.h"
#include "WifiService.h"

#include <rfs/RootFileSystem.h>

// for disabling brown-out detector
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

#include <utils/Log.h>

using namespace Rfs;

ChairControllerApp::ChairControllerApp() :
        Base(), remoteCtrl(0), mUdpServer(), mChairController(), mRearLeftCfg(), mRearRightCfg(), mFrontCfg(), mRotationCfg() {
}

ChairControllerApp::~ChairControllerApp() {
}

void ChairControllerApp::Init() {
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

    Logger::Init(Logger::LogSink::Serial);

    // TODO: Remove debug code for production
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);

    // TODO Remove
	// Serial.begin(115200);
	// Serial.setDebugOutput(true);
	// Serial.println();
    // Serial.println("Test");
    WifiService::GetInstance()->StartWifi();
	RootFileSystem* rfs = RootFileSystem::GetInstance();
	if (rfs != 0) {
		rfs->Start();
	}

    // TODO: Create constants for the pins to make the code better readable
    // TODO: Apply the microstep config from stepper driver
    mRearLeftCfg  = MoverDriverCfg::Create("REAR_LEFT", 33, 14, 36, 39, 35, 4);
    mRearRightCfg = MoverDriverCfg::Create("REAR_RIGHT", 18, 26, 5, 23, 19, 4);
    // TODO Enable the commented line below for production!
    // Pin 2 is replaced by pin 36 during debugging, because it affects the internal LED!
    //mFrontCfg     = MoverDriverCfg::Create(21, 22, 2,  16, 17);
    mFrontCfg     = MoverDriverCfg::Create("FRONT", 21, 22, 36,  16, 17, 4);
    mRotationCfg  = MoverDriverCfg::Create("ROTATION", 12, 4, 27, 25, 32, 4);


    InitializeChairController();
    StartVrConnection();
    StartRemoteCtrl();
}

void ChairControllerApp::StartRemoteCtrl() {
    remoteCtrl = RemoteCtrl::GetInstance();
    remoteCtrl->Init(mChairController);
}

void ChairControllerApp::InitializeChairController() {
    MoverDriverPtr rearLeft = MoverDriver::Create(mRearLeftCfg);
    MoverDriverPtr rearRight = MoverDriver::Create(mRearRightCfg);
    MoverDriverPtr front = MoverDriver::Create(mFrontCfg);
    MoverDriverPtr rotation = MoverDriver::Create(mRotationCfg);

    mChairController = ChairController::Create(rearLeft, rearRight, front, rotation);
}

void ChairControllerApp::StartVrConnection() {
    mUdpServer = UdpServer::Create();
    mUdpServer->Init(mChairController);
    Attach(mUdpServer);
}

void ChairControllerApp::ProcessEvents() {
    mUdpServer->ReceiveData();
}
