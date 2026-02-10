#include "UdpServer.h"
#include <TaskMgmt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <utils/Log.h>

UdpServer::UdpServer() : 
        mServerTask(nullptr), mUdp(), mChairController() {
}

UdpServer::~UdpServer() {
    // task will stop and delete itself
    mServerTask = nullptr;
}

UdpServerPtr UdpServer::Create() {
    return ::std::shared_ptr<UdpServer>(new UdpServer());
}

void UdpServer::Init(ChairControllerPtr chairController) {
    mChairController = chairController;
    LogInfo("UDP Server: Initialized on port 4210");
}

void UdpServer::Start() {
    if (mServerTask == nullptr) {
        mServerTask = TaskMgmt::CreateTask(
            TaskMgmt::TaskId::UDP_SERVER,
            [](void* pvParameters) { // Task function - lambda
                    UdpServer* server = static_cast<UdpServer*>(pvParameters);
                    LogDbg("UDP Server: Started");
                    server->Listen();
            },
            this
        );
        // TaskMgmt::TaskConfig cfg = TaskMgmt::GetConfig(TaskMgmt::TaskId::UDP_SERVER);
        // BaseType_t res = xTaskCreatePinnedToCore(
        //     [](void* pvParameters) { // Task function - lambda
        //             UdpServer* server = static_cast<UdpServer*>(pvParameters);
        //             LogDbg("UDP Server: Started");
        //             server->Listen();
        //     },
        //     cfg.label,         // Task name
        //     cfg.stackDepth,    // Stacksize
        //     this,              // Parameter - reference to the instance
        //     cfg.priority,      // Priority
        //     &mServerTask,      // Task handle
        //     cfg.core           // pin to core
        // );
        // if (res != pdPASS) {
        //     LogError("UDP Server: Failed to create server task");
        // }
    } else {
        LogWarn("UDP Server: Skipped recreation of server task");
    }
}

void UdpServer::Listen() {
    const uint32_t bufferLength = 64;
    unsigned char buffer[bufferLength];
    mUdp.begin(4210);

    while (mServerTask != nullptr && mUdp.parsePacket()) {
        uint32_t bytesRead = mUdp.read(buffer, bufferLength);
        //LogDbg("Bytesread %d buffer %d %d %d", bytesRead, buffer[0], buffer[1], buffer[2]);

        if (bytesRead == 32) {
            uint32_t* data = (uint32_t*) buffer;
            uint32_t timeStamp = *data;
            SimulationData::Mode mode = (SimulationData::Mode) *(++data);
            uint32_t pitch = *(++data);
            uint32_t yaw = *(++data);
            uint32_t roll = *(++data);
            uint32_t posX = *(++data);
            uint32_t posY = *(++data);
            uint32_t posZ = *(++data);
            SimulationData simulationData(timeStamp, mode, pitch, yaw, roll, posX, posY, posZ);
            LogDbg("UDP Server: Received %d %d %d %d %d %d %d %d", timeStamp, mode, pitch, yaw, roll, posX, posY, posZ);

            if (mChairController != nullptr) {
                mChairController->AdjustToSimulation(simulationData);
            }
        } else {
            LogError("UDP Server: Not enough bytes read");
        }
    }
    vTaskDelete(NULL);
}
