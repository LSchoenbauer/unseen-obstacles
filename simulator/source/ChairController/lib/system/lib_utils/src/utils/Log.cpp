/*
 * ===============================================================================
 * (c) HTL Leonding
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

#include <utils/Log.h>
#include <system/SystemTime.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hardwareSerial.h>

const char* Logger::mFilePathStarter = 0;

uint8_t Logger::mEnabledSinks = 0;

#ifdef DO_LOG_MULTITHREADED
	#pragma message	("LOG: multithreaded logging enabled!")
	QueueHandle_t Logger::mLogQueue = 0;
	SemaphoreHandle_t Logger::mLogMutex = 0;
	
	const uint32_t Logger::mLogBufferLength = 2048;
	char Logger::mLogBuffer[mLogBufferLength];
	size_t Logger::mNextFreeLogBufferIdx = 0;
	size_t Logger::mLogBufferStartIdx = 0;
#else
	#pragma message	("LOG: SINGLE threaded logging only!")
#endif

void Logger::Init(uint8_t sinksToEnable) {
	// single vs. multithreaded logging is handled in method
	CreateLoggingTask();

	for (uint8_t sink = 1; sink < 8; sink <<= 1) { 
		// run through all possible sink bits
		if (sinksToEnable & sink) {
			SetSinkEnabled(static_cast<LogSink::Enum>(sink), true);
		}
	}
}

void Logger::SetSinkEnabled(LogSink::Enum sink, bool enable) {

	if (enable) {
		mEnabledSinks |= sink;
		switch (sink) {
			case LogSink::Serial:
				Serial.begin(115200);
				Serial.setDebugOutput(IsLogDbgEnabled());
				break;
			default:
				break;
		}
	} else {
		mEnabledSinks &= ~sink;
		switch (sink) {
			case LogSink::Serial:
				Serial.end();
				break;
			default:
				break;
		}
	}
}

bool Logger::IsSinkEnabled(LogSink::Enum sink) { 
	return (mEnabledSinks & sink); 
}

void Logger::LogLineBreak() {
	Logger::PrintLogMsg(0, true, 0, 0, 0, 0, 0, 0);
}

void Logger::Log(const char* level, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(level, false, 0, 0, 0, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(const char* level, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(level, true, 0, 0, 0, 0, msg, &args);
	va_end(args);
}

void Logger::Log(const char* level, const char* file, const char* fct, int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(level, false, file, fct, line, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(const char* level, const char* file, const char* fct, int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(level, true, file, fct, line, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(const char* level, const Rc& rc, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(level, true, 0, 0, 0, &rc, msg, &args);
	va_end(args);
}

void Logger::LogLn(const char* level, const char* file, const char* fct, int line, const Rc& rc, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(level, true, file, fct, line, &rc, msg, &args);
	va_end(args);
}

void Logger::LogLnIfFail(bool rc, const char* level, const char* msg, ...) {
	if (!rc) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(level, true, 0, 0, 0, 0, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(bool rc, const char* level, const char* file, const char* fct, int line, const char* msg, ...) {
	if (!rc) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(level, true, file, fct, line, 0, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(const Rc& rc, const char* level, const char* msg, ...) {
	if (!rc.IsOk()) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(level, true, 0, 0, 0, &rc, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(const Rc& rc, const char* level, const char* file, const char* fct, int line, const char* msg, ...) {
	if (!rc.IsOk()) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(level, true, file, fct, line, &rc, msg, &args);
		va_end(args);
	}
}

const char* Logger::Dump(bool value) {
	return value ? "YES" : "NO";
}

const char* Logger::Dump(const String& value) {
	return value ? value.c_str() : "INV";
}

// void Logger::LogHeapStatistix() -- board specific implementation

void Logger::ShortenFilePath(const char* trimInclusive) {
	Logger::mFilePathStarter = trimInclusive;
}

const char* Logger::GetFileName(const char* file) {
	const char* nameStart = strrchr(file, '/');
	return nameStart != 0 ? nameStart + (sizeof(char)) : file;
}

const char* Logger::GetFilePath(const char* file) {
	const char* pathStart = 0;
	if (Logger::mFilePathStarter != 0) {
		pathStart = strstr(file, Logger::mFilePathStarter);
	}
	return pathStart != 0 ? pathStart + (strlen(Logger::mFilePathStarter)) : file;
}

int Logger::CalcGap(int minLocatorLen, const char* file, const char* fct, int line) {
	int len = (strlen(file) + strlen(fct) + (line < 10 ? 1 : (line < 100 ? 2 : (line < 1000 ? 3 : 4))) + 5);
	return minLocatorLen > len ? minLocatorLen - len : 1;
}

void Logger::PrintLogMsg(const char* level, bool newLn, const char* file, const char* fct, int line, const Rc* rc,
        const char* msg, va_list* args) {
	String str;
	str.reserve(128);

	if (level != 0) {
		str.concat(level);
		str.concat(' ');
	}

	if (newLn && msg != 0) {
		char timeBuffer[16];
		uint64_t sysTime = ::System::SystemTime::GetMicroseconds64();
		sprintf(timeBuffer, "%03u.%03u.%03u",
			(uint32_t)(sysTime / 1000000l), 
			(uint32_t)((sysTime % 1000000l) / 1000l), 
			(uint32_t)(sysTime % 1000l));
		str.concat(timeBuffer);
		str.concat(' ');
	}

	char buffer[__LogLocatorMinLen__ + 16];
	char* bufferPtr = buffer;
	if (file != 0 && fct != 0) {
		const char* fName = Logger::GetFileName(file);
		const char* fPath = Logger::GetFilePath(file);
		int gap = Logger::CalcGap(__LogLocatorMinLen__, fPath, fct, line);
		size_t strLen = snprintf(buffer, sizeof(buffer), "%s%s::%s #%d:%*s", (file != fPath ? "..." : ""), fPath, fct, line, gap, " ");
		if (strLen > sizeof(buffer) - 1) {
			bufferPtr = new char[strLen + 1];
			if (bufferPtr != 0) {
				strLen = snprintf(bufferPtr, strLen + 1, "%s%s::%s #%d:%*s", (file != fPath ? "..." : ""), fPath, fct, line, gap, " ");
			}
		}
		str.concat(bufferPtr);
		if (buffer != bufferPtr) {
			delete[] bufferPtr;
		}
	}

	if (msg != 0) {
		bufferPtr = buffer;
		// note: va_list is started and ended in wrapping functions!
		size_t strLen = vsnprintf(buffer, sizeof(buffer), msg, *args);
		if (strLen > sizeof(buffer) - 1) {
			bufferPtr = new char[strLen + 1];
			if (bufferPtr != 0) {
				vsnprintf(bufferPtr, strLen + 1, msg, *args);
			}
		}
		str.concat(bufferPtr);
		if (buffer != bufferPtr) {
			delete[] bufferPtr;
		}
	}

	if (rc != 0) {
		str.concat(" [#");
		str.concat(rc->GetCode());
		str.concat(": ");
		str.concat(rc->GetName());
		str.concat(']');
	}
	
	if (newLn) {
		str.concat(__LogLb);
	}

	Logger::DoLogMessage(str.c_str());
}

void Logger::DoLogMessage(const char* msg) {
#ifdef DO_LOG_MULTITHREADED
	EnqueueLog(msg);
#else
	Logger::ToSerial(msg);
	// add additional log sinks here
#endif
}

void Logger::ToSerial(const char* msg) {
	if (Logger::IsSinkEnabled(LogSink::Serial)) {
		Serial.print(msg);
	}
}

void Logger::CreateLoggingTask() {
#ifdef DO_LOG_MULTITHREADED
	if (mLogMutex == 0) {
		mLogMutex = xSemaphoreCreateMutex();
		mLogQueue = xQueueCreate(10, sizeof(LogEvent));

		xTaskCreatePinnedToCore(
			Logger::LoggingTask,
			"LoggingTask",
			2048,           // Stack size
			NULL,           // Parameter
			1,              // Priority
			NULL,           // Task-Handle
			1               // pinned to core 1
		);
	}
#endif
}

#ifdef DO_LOG_MULTITHREADED

void Logger::EnqueueLog(const char* msg) {
	if (xSemaphoreTake(mLogMutex, portMAX_DELAY) == pdTRUE) {
		size_t msgLen = strlen(msg) + 1;
		char* buffer = (char*)pvPortMalloc(msgLen);
		strncpy(buffer, msg, msgLen);

		Logger::LogEvent logEvent = { buffer };
		xQueueSend(Logger::mLogQueue, &logEvent, portMAX_DELAY);
		xSemaphoreGive(mLogMutex);
	}
}

void Logger::LoggingTask(void *pvParameters) {
  Logger::LogEvent logEvent;
  while (true) {
    if (xQueueReceive(Logger::mLogQueue, &logEvent, portMAX_DELAY) == pdTRUE) {
      Logger::ToSerial(logEvent.msg);
	  // add additional log sinks here

	  vPortFree((void*)(logEvent.msg));
    }
  }
}

#endif
