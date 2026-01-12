#include <utils/Log.h>
#include "UdpServer.h"

UdpServer::UdpServer() : mUdp(), mChairController() {}

UdpServer::~UdpServer(){}

UdpServerPtr UdpServer::Create() {
	return ::std::shared_ptr<UdpServer>(new UdpServer());
}

void UdpServer::Init(ChairControllerPtr chairController) {
  mChairController = chairController;

  mUdp.begin(4210);
  LogInfo("Started Udp Server");
}

void UdpServer::OnEvent(std::shared_ptr<AppEvent> ev) {
  if (mChairController != nullptr) {
    ReceiveData();
  }
}

void UdpServer::ReceiveData() {
  const uint32_t bufferLength = 64;
  unsigned char buffer[bufferLength];

  if (mUdp.parsePacket()) {
    uint32_t bytesRead = mUdp.read(buffer, bufferLength);

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

      if (mChairController != nullptr) {
        mChairController->AdjustToSimulation(simulationData);
      }
    } else {
      LogError("Not enough bytes read");
    }
  }
}

