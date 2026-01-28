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

    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);


	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println();
    Serial.println("Test");
    WifiService::GetInstance()->StartWifi();
	RootFileSystem* rfs = RootFileSystem::GetInstance();
	if (rfs != 0) {
		rfs->Start();
	}

    mRearLeftCfg = MoverDriverCfg::Create(33, 14, 36, 39, 35);
    mRearRightCfg = MoverDriverCfg::Create(18, 26, 5, 23, 19);
    mFrontCfg = MoverDriverCfg::Create(21, 22, 2,  16, 17);
    mRotationCfg = MoverDriverCfg::Create(12, 4, 27, 25, 32);

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
