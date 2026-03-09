#include "ChairControllerApp.h"
#include "WifiService.h"
#include "MoverDriver.h"

#include <os/TaskMgmt.h>
#include <TaskConfigs.h>
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

	// Initialize logging and set log levels
    Logger::Init(Logger::LogSink::Serial);
	SetMaxLogTagLevel(UdpServer::LTAG_RX, LOG_LEVEL_WARN);
	SetMaxLogTagLevel(UdpServer::LTAG_TX, LOG_LEVEL_WARN);
	SetMaxLogTagLevel(MoverDriver::LTAG_CFG, LOG_LEVEL_DEBUG);
	SetMaxLogTagLevel(MoverDriver::LTAG_CTR, LOG_LEVEL_DEBUG);
	SetMaxLogTagLevel(MoverDriver::LTAG_DRV, LOG_LEVEL_WARN);

    WifiService::GetInstance()->StartWifi();
	RootFileSystem* rfs = RootFileSystem::GetInstance();
	if (rfs != 0) {
		rfs->Start();
	}

    // TODO: Create constants for the pins to make the code better readable
    // TODO: Apply the microstep config from stepper driver
    mRearLeftCfg  = MoverDriverCfg::Create("REAR_LEFT", 33, 14, 36, 39, 35, 2);
    //mRearLeftCfg  = MoverDriverCfg::Create("REAR_LEFT", 12, 4, 27, 25, 32, 2); // TODO: now left rear was moving

    mRearRightCfg = MoverDriverCfg::Create("REAR_RIGHT", 18, 26, 5, 23, 19, 2);
    // TODO Enable the commented line below for production!
    // Pin 2 is replaced by pin 36 during debugging, because it affects the internal LED!
    //mFrontCfg     = MoverDriverCfg::Create("FRONT", 21, 22, 2,  16, 17, 2);
    mFrontCfg     = MoverDriverCfg::Create("FRONT", 21, 22, 16,  16, 17, 2);
    mRotationCfg  = MoverDriverCfg::Create("ROTATION", 12, 4, 27, 25, 32, 2); //12, 4, 27, 25, 32, 2

    InitializeChairController();
    StartVrConnection();
    StartRemoteCtrl();

    StartTopInTask(TaskConfigs::Get(TaskConfigs::TaskCfgId::TASK_TABLE_DUMPER), 5000);
}

void ChairControllerApp::InitializeChairController() {
    MoverDriverPtr rearLeft = MoverDriver::Create(mRearLeftCfg);
    MoverDriverPtr rearRight = MoverDriver::Create(mRearRightCfg);
    MoverDriverPtr front = MoverDriver::Create(mFrontCfg);
    MoverDriverPtr rotation = MoverDriver::Create(mRotationCfg);

    mChairController = ChairController::Create(rearLeft, rearRight, front, rotation);

    uint32_t initialPriority = uxTaskPriorityGet(NULL);
    TaskMgmt::TaskConfig taskCfg = TaskConfigs::Get(TaskConfigs::TaskCfgId::CHAIR_CONTROLLER);
    if (initialPriority != taskCfg.priority) {
        vTaskPrioritySet(NULL, taskCfg.priority);
        LogInfo("ChairControllerApp: Changed priority of main task from %d to %d", initialPriority, taskCfg.priority);
    }
    Attach(mChairController);
}

void ChairControllerApp::StartVrConnection() {
// TODO: Design flaw - better to move the body of this method to a separate adapter class, similar to "RemoteCtrl"
    mUdpServer = UdpServer::Create(VR_SERVER_PORT);
    mUdpServer->OnPacket([this](const uint8_t* data, size_t dataSize) {
        OnVrDataReceived(data, dataSize);
    });
    mUdpServer->Start(TaskConfigs::Get(TaskConfigs::TaskCfgId::UDP_SERVER));
}

void ChairControllerApp::StartRemoteCtrl() {
    remoteCtrl = RemoteCtrl::GetInstance();
    remoteCtrl->Init(mChairController);
}

void ChairControllerApp::ProcessEvents() {
    mChairController->OnEvent(nullptr);
}

// TODO: Design flaw - better to move this method to a separate adapter class, similar to "RemoteCtrl"
void ChairControllerApp::OnVrDataReceived(const uint8_t* data, size_t dataLength) {
    const char* ping = "PING";
    const size_t pingDataLen = strlen(ping);
    const char* response = "PONG";
    const size_t responseDataLen = strlen(response);

    if (data != nullptr) {
        if (dataLength == pingDataLen && strncmp(reinterpret_cast<const char*>(data), ping, pingDataLen) == 0) {
            LogError("ChairControllerApp: Received protocol signal: %.*s (%d)", dataLength, data, dataLength);
            mUdpServer->Respond(reinterpret_cast<const uint8_t*>(response), responseDataLen);
        } else if (dataLength == 32) {
            const uint32_t* vrData = reinterpret_cast<const uint32_t*>(data);
            // the order of the following lines is important because of the binary data format
            uint32_t timeStamp = *vrData;
            SimulationData::Mode mode = (SimulationData::Mode) *(++vrData);
            uint32_t pitch = *(++vrData);
            uint32_t yaw = *(++vrData);
            uint32_t roll = *(++vrData);
            uint32_t posX = *(++vrData);
            uint32_t posY = *(++vrData);
            uint32_t posZ = *(++vrData);
            SimulationData simulationData(timeStamp, mode, pitch, yaw, roll, posX, posY, posZ);
            LogDbg("ChairControllerApp: Received %d %d %d %d %d %d %d %d", timeStamp, mode, pitch, yaw, roll, posX, posY, posZ);

            if (mChairController != nullptr) {
                mChairController->AdjustToSimulation(simulationData);
            }
        } else {
            LogError("ChairControllerApp: Parsing VR data failed: Invalid length");
        }
    } else {
        LogWarn("ChairControllerApp: No data received (NULL).");
    }
}