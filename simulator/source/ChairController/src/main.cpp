/*
 * ===============================================================================
 * c) HTL Leonding
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

#include <Arduino.h>
#include "ChairControllerApp.h"
#include <TaskConfigs.h>
#include <os/TaskMgmt.h>
#include <utils/Log.h>

using namespace Web;
using namespace Web::Http;
 
ChairControllerApp* gTheApp = 0;

// ESP-IDF only section
#ifdef ESPIDF_BASED

#if CONFIG_FREERTOS_UNICORE
	#define yieldIfNeeded() yieldIfNecessary()

	void yieldIfNecessary(void){
		static uint64_t lastYield = 0;
		uint64_t now = millis();
		if((now - lastYield) > 2000) {
			lastYield = now;
			vTaskDelay(5); //delay 1 RTOS tick
		}
	}
#else
	#define yieldIfNeeded()
#endif

void mainTaskFn(void *pvParameters) {
	setup();
	for(;;) {
		yieldIfNeeded();
		loop();
		if (serialEventRun) {
			 serialEventRun();
		}
	}
}

extern "C" void app_main() {
	initArduino();
    Logger::Init(Logger::LogSink::Serial);
	LogInfo("System started");
	TaskMgmt::TaskConfig mainTaskCfg = TaskConfigs::Get(TaskConfigs::TaskCfgId::CHAIR_CONTROLLER);
	TaskMgmt::CreateTask(mainTaskCfg, mainTaskFn, NULL);
}

#endif // ESPIDF_BASED

// Arduino entry points, kept for compatibility with Arduino framework

void setup() {
	gTheApp = ChairControllerApp::GetInstance();
	gTheApp->Init();
}

void loop() {
	gTheApp->ProcessEvents();
}
