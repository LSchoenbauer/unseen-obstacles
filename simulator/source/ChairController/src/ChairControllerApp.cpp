#include "ChairControllerApp.h"
#include "WifiService.h"

#include <rfs/RootFileSystem.h>

// for disabling brown-out detector
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

using namespace Rfs;

ChairControllerApp::ChairControllerApp() :
        Base(), remoteCtrl(0) {
}

ChairControllerApp::~ChairControllerApp() {
}

void ChairControllerApp::Init() {
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println();
    Serial.println("Test");
    WifiService::GetInstance()->StartWifi();
	RootFileSystem* rfs = RootFileSystem::GetInstance();
	if (rfs != 0) {
		rfs->Start();
	}
    StartRemoteCtrl();
}

void ChairControllerApp::StartRemoteCtrl() {
    Serial.println("Test2");
    remoteCtrl = RemoteCtrl::GetInstance();
    remoteCtrl->Init();
    Serial.println("Test3");
}