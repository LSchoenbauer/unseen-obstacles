#include <Arduino.h>
#include <memory>

#include "WifiService.h"

#include <rfs/RootFileSystem.h>

#include <appfw/AppComponent.h>
#include <web/MimeTypes.h>
#include <utils/Log.h>

// for disabling brown-out detector
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

#include <web/http/HttpServer.h>

using namespace Rfs;
using namespace Web;
using namespace Web::Http;

const int PIN_PULSE1 = 12;     // connected to "PULSE" of the stepper driver
const int PIN_DIR1 = 4;       // connected to "DIR" of the stepper driver
const int PIN_SPEED_IN = 34;  // connected to poti for steering the speed

const int PIN_PULSE2 = 18;
const int PIN_DIR2 = 26;

const uint32_t DIR_CHANGE_LEAD_TIME_US = 10; // direction change must be applied 10 µs before the next pulse.
const uint32_t FULL_REVOLUTION_STEP_COUNT = 200;
const uint32_t MICROSTEP_FACTOR = 4;
const uint32_t PULSE_DUTY_CYCLE_PC = 50;
const uint32_t MIN_PULSE_DURATION_US = 10;

const uint32_t SPEED_TRANSITION_STEPS = 100; // The number of steps to ramp up or down speed changes
const uint32_t STEPS_PER_DIR = 3200;   // The number of steps for running in the same direction

const uint32_t STEPS_PER_MOVE = 1000;

uint32_t currentSpeedRpm = 0;         // The current speed in revolutions per minute
uint32_t stepCount = 0;               // Counter for steps in a single direction

bool move = false;
bool dirUp = false;

// Define a struct to hold the message and arguments to be printed to terminal
struct SerialMessage {
  size_t GetCapacity() const {return mTextCapcity; }
  char* GetText() { return mText; };
  
  private:
    static const size_t mTextCapcity = 128;
    char mText[mTextCapcity];
};

/** Prints the "format" text, which can be formatted like 'printf' as single line to standard out */
void stdPrintfLn(const char* format, ...);

// Queue handle for serial output
QueueHandle_t serialQueue;
// Task handle for stepper driver task
TaskHandle_t stepperTaskHandle = NULL;
// Task handle for serial output task
TaskHandle_t serialTaskHandle = NULL;
// Function to handle serial output

void stepperTask(void *args);
void serialOutputTask(void *args);
HttpServerPtr mHttpServer; // global
void setUpWeb() {

	// HTTP server

	// root directory for public content - should be retrieved from configuration
	String docRoot("/ffs/www");

	// create the server instance
	mHttpServer = HttpServer::Create(docRoot);

	// add a simple 'ping' REST endpoint
	mHttpServer->OnRequest("/ping", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogDbg("HTTP: Received PING");
		req->SendContent("<!DOCTYPE html><html><body>PONG</body></html>", MimeType::HTML);
	});

	mHttpServer->OnRequest("/clickMe", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button clicked");
		req->SendStatus(200);
	});

	// Motor 1
	mHttpServer->OnRequest("/motor1Up", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 1 UP clicked");
    move = true;
    dirUp = true;
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor1Center", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 1 CENTER clicked");
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor1Down", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 1 DOWN clicked");
    move = true;
    dirUp = false;
		req->SendStatus(200);
	});


	// Motor 2
	mHttpServer->OnRequest("/motor2Up", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 2 UP clicked");
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor2Center", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 2 CENTER clicked");
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor2Down", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 2 DOWN clicked");
		req->SendStatus(200);
	});


	// Motor 3
	mHttpServer->OnRequest("/motor3Up", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 3 UP clicked");
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor3Center", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 3 CENTER clicked");
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor3Down", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 3 DOWN clicked");
		req->SendStatus(200);
	});


	// Motor 4
	mHttpServer->OnRequest("/motor4Up", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 4 UP clicked");
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor4Center", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 4 CENTER clicked");
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor4Down", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 4 DOWN clicked");
		req->SendStatus(200);
	});


	// configure static content after other handlers to avoid searching pages that are handled by others
	mHttpServer->SetIndexPage("/index.html");
	mHttpServer->SetStaticContentRoot("/", "/");

	// eventually start the server
	mHttpServer->Start();

	LogInfo("");
	LogInfo("----------------------------------------------------");

	LogInfo("RemoteCtrl initialized");
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  pinMode(PIN_PULSE1, OUTPUT);
  pinMode(PIN_DIR1, OUTPUT);
  pinMode(PIN_PULSE2, OUTPUT);
  pinMode(PIN_DIR2, OUTPUT);
  digitalWrite(PIN_DIR1, LOW); // Initially LOW
  digitalWrite(PIN_DIR2, LOW); // Initially LOW
  
  // Create a queue for serial output
  serialQueue = xQueueCreate(128, sizeof(SerialMessage));

  // Create a task for driving the stepper on Core 0
  // xTaskCreatePinnedToCore(
  //   stepperTask, // Function to implement the task
  //   "StepperTask", // Name of the task
  //   10000, // Stack size in words
  //   NULL, // Task input parameter
  //   1, // Priority of the task
  //   &stepperTaskHandle, // Task handle
  //   0 // Core where the task should run (Core 1)
  // );

  // Create a task for serial output on Core 1
  xTaskCreatePinnedToCore(
    serialOutputTask, // Function to implement the task
    "SerialOutputTask", // Name of the task
    10000, // Stack size in words
    NULL, // Task input parameter
    1, // Priority of the task
    &serialTaskHandle, // Task handle
    0 // Core where the task should run (Core 0)
  );


	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println();
    Serial.println("Test");
    WifiService::GetInstance()->StartWifi();
	RootFileSystem* rfs = RootFileSystem::GetInstance();
	if (rfs != 0) {
		rfs->Start();
	}
    setUpWeb();
}

/**
 * Provides the user controlled speed in rounds per minute.
 */
uint32_t getTargetSpeedRpm() {
  static int32_t stepsLeft = 0;
  static uint32_t appliedValue = 0;
  // Read the potentiometer value
  uint32_t analogSpeedValue = 2500;

  // calculate and print statistics
  static uint32_t minVal = 5000;
  static uint32_t maxVal = 0;
  static uint32_t sum = 0;
  static uint32_t count = 0;

  if (count > STEPS_PER_DIR) {
    minVal = 5000;
    maxVal = 0;
    sum = 0;
    count = 0;
  }

  if (move) {
    stepsLeft = STEPS_PER_MOVE;
    move = false;
  }

  if (stepsLeft >= 0) {
  --stepsLeft;
  }

  // minVal = analogSpeedValue < minVal ? analogSpeedValue : minVal;
  // maxVal = analogSpeedValue > maxVal ? analogSpeedValue : maxVal;
  // sum += analogSpeedValue;
  // count++;
  // double avg = ((double)sum) / count;
  // uint32_t devFloor = 100 - ((minVal * 100) / avg);
  // uint32_t devCeil = ((maxVal * 100) / avg) - 100;

  // if (count % 100 == 0) {
  //   // stdPrintfLn("Analog speed value: min: %d, max: %d, avg: %.1f, dev: -%d%%/+%d%%", minVal, maxVal, avg, devFloor, devCeil);
  // }

  // // Convert the potentiometer reading to time activePulseDurationUs
  // // Assuming the potentiometer returns a value between 0 and 4095
  // if (analogSpeedValue > appliedValue * 1.05 || analogSpeedValue < appliedValue * 0.95) {
  //   appliedValue = analogSpeedValue;
  // }

  if (stepsLeft <= 0) {
    appliedValue = 0;
  } else {
    appliedValue = 60;
  }

  // stdPrintfLn("Applied speed value: %d, stepsLeft: %d", appliedValue, stepsLeft);
  //return (uint32_t) map(appliedValue, 0, 4095, 6, 300);
  return appliedValue;
}

/**
 * Provides the duration of a complete pulse (high AND low phase)
 */
uint32_t getPulseDurationUs(uint32_t speedRpm) {
  // Counter for steps in linear transition of pulse duration changes
  static uint32_t transitionStep = 0;
  static double transitionStartSpeedRpm = 0;    // the speed at start of the transition
  static double transitionStepSpeedRpm = 0;     // the speed change per transition step
  static uint32_t targetSpeedRpm = 0;             // the target speed in revolutions per minute

  // Check if target speed has changed and apply linear transition
  if (targetSpeedRpm != speedRpm) {
    transitionStartSpeedRpm = currentSpeedRpm;
    targetSpeedRpm = speedRpm;
    transitionStepSpeedRpm = (targetSpeedRpm - transitionStartSpeedRpm) / SPEED_TRANSITION_STEPS;
    transitionStep = 0;
  }

  // Apply linear transition for pulse duration if not yet at target pulse duration
  bool printChange = false;
  if (transitionStep <= SPEED_TRANSITION_STEPS) {
    currentSpeedRpm = transitionStep == SPEED_TRANSITION_STEPS 
     ? targetSpeedRpm
     : (uint32_t)((transitionStepSpeedRpm * transitionStep) + transitionStartSpeedRpm);
    transitionStep++;
    printChange = true; //transitionStep % 10 == 0 || transitionStep == SPEED_TRANSITION_STEPS;
  }
  
  // Calculate the duration of a complete pulse in µs
  uint32_t stepsPerMinute = (currentSpeedRpm * FULL_REVOLUTION_STEP_COUNT * MICROSTEP_FACTOR);
  uint32_t pulseDuration = stepsPerMinute != 0 ? (60e6) / stepsPerMinute : 0;

  //printChange = false;
  if (printChange) {
    stdPrintfLn("Target speed: %d rpm, current speed: %d rpm (step: %d, f: %d Hz, tPulse: %d µs, spm: %d)", 
      targetSpeedRpm, currentSpeedRpm, transitionStep, pulseDuration != 0 ? (1000000 / pulseDuration) : 0, pulseDuration, stepsPerMinute);
  }

  return pulseDuration;
}

/**
 * Toggles the direction if the current step count exceeds the given limit.
 * @return True, if speed 0 RPM is expected to execute the direction change
 */
bool setDirection() {
  static bool wasUp = dirUp;
    //stdPrintfLn("wasUP: %s, dirUp: %s", (wasUp ? "UP" : "DOWN"), (dirUp ? "UP" : "DOWN"));
  // check if a direction change is required;
  bool changeDir = dirUp != wasUp;
  if (changeDir && currentSpeedRpm == 0) {
    wasUp = dirUp;
    stepCount = 0;

    // toggle the pin
    digitalWrite(PIN_DIR1, dirUp ? HIGH : LOW);
    digitalWrite(PIN_DIR2, dirUp ? HIGH : LOW);

    // direction change must be ahead of the next pulse
    delayMicroseconds(DIR_CHANGE_LEAD_TIME_US);

    // print rotation change
    stdPrintfLn("Dir changed to %s", (dirUp ? "UP" : "DOWN"));
  }
  return changeDir;
}

void step(uint32_t pulseDurationUs) {
  if (currentSpeedRpm > 0 && pulseDurationUs >= MIN_PULSE_DURATION_US) {
    uint32_t highDurationUs = (pulseDurationUs * PULSE_DUTY_CYCLE_PC) / 100;
    uint32_t lowDurationUs = pulseDurationUs - highDurationUs;

    digitalWrite(PIN_PULSE1, HIGH);
    digitalWrite(PIN_PULSE2, HIGH);
    delayMicroseconds(highDurationUs);
    digitalWrite(PIN_PULSE1, LOW);
    digitalWrite(PIN_PULSE2, LOW); //TODO
    delayMicroseconds(lowDurationUs);
    stepCount++;
  } else {
    delayMicroseconds(1000);
  }
}

void loop() {
  // delay(1000);
  stepperTask(0);
}

void stepperTask(void *args) {
  // change the direction of the stepper rotation
  bool changingDir = setDirection();

  // set to speed to 0 if direction change is ahead or read the new target speed
  uint32_t newTargetSpeedRpm = changingDir ? 0 : getTargetSpeedRpm();

  // calculate the total pulse duration
  uint32_t pulseDurationUs = getPulseDurationUs(newTargetSpeedRpm);

  // proceed the stepper
  step(pulseDurationUs);
}

// Formats  the given text with the given arguments and prints it to standard output.
void stdPrintfLn(const char* format, ...) {
    SerialMessage msg;

    va_list args;
    va_start(args, format);
    vsnprintf(msg.GetText(), msg.GetCapacity(), format, args);
    va_end(args);
  
    xQueueSend(serialQueue, &msg, 0);
}

// Retrieves messages and prints them to standard output.
void serialOutputTask(void *args) {
  SerialMessage msg;
  while (true) {
    if (xQueueReceive(serialQueue, &msg, portMAX_DELAY) == pdTRUE) {
      if (msg.GetText() != 0) {
        Serial.println(msg.GetText());
      }
    }
  }
}
