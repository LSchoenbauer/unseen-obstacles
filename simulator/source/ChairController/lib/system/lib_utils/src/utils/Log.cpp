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
const Logger::Tag Logger::mNoTag = 0;

#ifdef DO_LOG_MULTITHREADED
	#pragma message	("LOG: multithreaded logging enabled!")
	QueueHandle_t Logger::mLogQueue = nullptr;
	SemaphoreHandle_t Logger::mLogMutex = xSemaphoreCreateRecursiveMutex();
	
	const uint32_t Logger::mLogBufferLength = 2048;
	char Logger::mLogBuffer[mLogBufferLength];
	size_t Logger::mNextFreeLogBufferIdx = 0;
	size_t Logger::mLogBufferStartIdx = 0;
	#define ENTER_CR if (mLogMutex == nullptr || xSemaphoreTakeRecursive(mLogMutex, portMAX_DELAY) == pdTRUE) {
	#define EXIT_CR if (mLogMutex != nullptr) { xSemaphoreGiveRecursive(mLogMutex); } }
#else
	#pragma message	("LOG: SINGLE threaded logging only!")
	#define ENTER_CR {
	#define EXIT_CR }
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

std::unordered_map<Logger::Tag, std::string>& Logger::GetTagLabels() {
	static std::unordered_map<Logger::Tag, std::string> tagLabels;
	return tagLabels;
}

std::unordered_map<Logger::Tag, Logger::Level>& Logger::GetTagLevels() {
	static std::unordered_map<Logger::Tag, Logger::Level> tagLevels;
	return tagLevels;
}

Logger::Tag Logger::RegisterTag(Tag tag, const char* label) {
	ENTER_CR
		if (label != nullptr && tag != mNoTag) {
			GetTagLabels()[tag] = label;
		}
	EXIT_CR
	return tag;
}

void Logger::SetMaxTagLevel(Tag tag, Level level) {
	ENTER_CR
		if (tag != mNoTag) {
			if (level == nullptr) {
				GetTagLevels().erase(tag);
			} else {
				GetTagLevels()[tag] = level;
			}
		}
	EXIT_CR
}

bool Logger::IsTagEnabled(Tag tag, Level level) {
	bool isEnabled = (tag == mNoTag) || level == nullptr;
	ENTER_CR
		isEnabled = isEnabled || GetTagLevels().count(tag) == 0; // not defined -> enabled
		if (!isEnabled) {
			isEnabled |= IsLevelAtLeast(level, GetTagLevels()[tag]);
		}
	EXIT_CR
	return isEnabled;
}

void Logger::LogLineBreak() {
	Logger::PrintLogMsg(mNoTag, 0, true, 0, 0, 0, 0, 0, 0);
}

void Logger::LogLineBreak(Tag tag) {
	Logger::PrintLogMsg(tag, 0, true, 0, 0, 0, 0, 0, 0);
}

void Logger::Log(Level level, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(mNoTag, level, false, 0, 0, 0, 0, msg, &args);
	va_end(args);
}

void Logger::Log(Tag tag, Level level, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(tag, level, false, 0, 0, 0, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(Level level, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(mNoTag, level, true, 0, 0, 0, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(Tag tag, Level level, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(tag, level, true, 0, 0, 0, 0, msg, &args);
	va_end(args);
}

void Logger::Log(Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(mNoTag, level, false, file, fct, line, 0, msg, &args);
	va_end(args);
}

void Logger::Log(Tag tag, Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(tag, level, false, file, fct, line, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg( mNoTag, level, true, file, fct, line, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(Tag tag, Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(tag, level, true, file, fct, line, 0, msg, &args);
	va_end(args);
}

void Logger::LogLn(Level level, const Rc& rc, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(mNoTag, level, true, 0, 0, 0, &rc, msg, &args);
	va_end(args);
}

void Logger::LogLn(Tag tag, Level level, const Rc& rc, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(tag, level, true, 0, 0, 0, &rc, msg, &args);
	va_end(args);
}

void Logger::LogLn(Level level, const char* file, const char* fct, int line, const Rc& rc, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(mNoTag, level, true, file, fct, line, &rc, msg, &args);
	va_end(args);
}

void Logger::LogLn(Tag tag, Level level, const char* file, const char* fct, int line, const Rc& rc, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	Logger::PrintLogMsg(tag, level, true, file, fct, line, &rc, msg, &args);
	va_end(args);
}

void Logger::LogLnIfFail(bool rc, Level level, const char* msg, ...) {
	if (!rc) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(mNoTag, level, true, 0, 0, 0, 0, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(Tag tag, bool rc, Level level, const char* msg, ...) {
	if (!rc) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(tag, level, true, 0, 0, 0, 0, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(bool rc, Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	if (!rc) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(mNoTag, level, true, file, fct, line, 0, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(Tag tag, bool rc, Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	if (!rc) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(tag, level, true, file, fct, line, 0, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(const Rc& rc, Level level, const char* msg, ...) {
	if (!rc.IsOk()) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(mNoTag, level, true, 0, 0, 0, &rc, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(Tag tag, const Rc& rc, Level level, const char* msg, ...) {
	if (!rc.IsOk()) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(tag, level, true, 0, 0, 0, &rc, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(const Rc& rc, Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	if (!rc.IsOk()) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(mNoTag, level, true, file, fct, line, &rc, msg, &args);
		va_end(args);
	}
}

void Logger::LogLnIfFail(Tag tag, const Rc& rc, Level level, const char* file, const char* fct, int line, const char* msg, ...) {
	if (!rc.IsOk()) {
		va_list args;
		va_start(args, msg);
		Logger::PrintLogMsg(tag, level, true, file, fct, line, &rc, msg, &args);
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

 bool Logger::IsLevelAtLeast(Level level, Level threshold) {
	int levelIdx = strstr(LogLevelOrder, level) - LogLevelOrder;
	int thresholdIdx = strstr(LogLevelOrder, threshold) - LogLevelOrder;
	return (levelIdx > 0 ) && (levelIdx >= thresholdIdx);
 }

void Logger::ShortenFilePath(const char* trimInclusive) {
	ENTER_CR
		Logger::mFilePathStarter = trimInclusive;
	EXIT_CR
}

const char* Logger::GetFileName(const char* file) {
	const char* nameStart = strrchr(file, '/');
	return nameStart != 0 ? nameStart + (sizeof(char)) : file;
}

const char* Logger::GetFilePath(const char* file) {
	const char* pathStart = 0;
	uint32_t offset = 0;
	ENTER_CR
		if (Logger::mFilePathStarter != 0) {
			pathStart = strstr(file, Logger::mFilePathStarter);
			offset = strlen(Logger::mFilePathStarter);
		}
	EXIT_CR
	return pathStart != 0 ? pathStart + offset : file;
}

int Logger::CalcGap(int minLocatorLen, const char* file, const char* fct, int line) {
	int len = (strlen(file) + strlen(fct) + (line < 10 ? 1 : (line < 100 ? 2 : (line < 1000 ? 3 : 4))) + 5);
	return minLocatorLen > len ? minLocatorLen - len : 1;
}

void Logger::PrintLogMsg(Tag tag, Level level, bool newLn, const char* file, const char* fct, int line, 
		const Rc* rc, const char* msg, va_list* args) {
	ENTER_CR
		if (IsTagEnabled(tag, level)) {

			// kept for debugging purposes, can be removed later
			// for (const auto& [tag, label] : Logger::GetTagLabels()) {
			// 	char buffer[128];
			// 	snprintf(buffer, sizeof(buffer), "--> Tag: %x, Label: %s\n", static_cast<int>(tag), label.c_str());
			// 	DoLogMessage(buffer);
			// }

			String str;
			str.reserve(128);

			if (level != 0) {
				str.concat(level);
				str.concat(' ');
			}

			if (newLn && msg != 0) {
				char timeBuffer[16];
				uint64_t sysTime = ::System::SystemTime::GetMicroseconds64();
				snprintf(timeBuffer, sizeof(timeBuffer), "%03u.%03u.%03u ",
					(uint32_t)(sysTime / 1000000l), 
					(uint32_t)((sysTime % 1000000l) / 1000l), 
					(uint32_t)(sysTime % 1000l));
				str.concat(timeBuffer);
			}

#ifdef DO_LOG_MULTITHREADED
			if (newLn && msg != 0) {
				char coreBuffer[8];
				uint8_t coreId = xPortGetCoreID();
				snprintf(coreBuffer, sizeof(coreBuffer), "C%u ", coreId);
				str.concat(coreBuffer);
			}
#endif

			if (tag != mNoTag && GetTagLabels().count(tag) > 0) {
				str.concat("[");
				str.concat(GetTagLabels()[tag].c_str());
				str.concat("] ");
			}

			char buffer[__LogLocatorMinLen__ + 16];
			char* bufferPtr = buffer;
			if (file != 0 && fct != 0) {
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
	EXIT_CR
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
	if (mLogQueue == nullptr) {
		mLogQueue = xQueueCreate(10, sizeof(LogEvent));
		xTaskCreatePinnedToCore(
			Logger::LoggingTask,
			"logger",
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
	ENTER_CR
		size_t msgLen = strlen(msg) + 1;
		char* buffer = (char*)pvPortMalloc(msgLen);
		strncpy(buffer, msg, msgLen);

		Logger::LogEvent logEvent = { buffer };
		xQueueSend(Logger::mLogQueue, &logEvent, portMAX_DELAY);
	EXIT_CR
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
