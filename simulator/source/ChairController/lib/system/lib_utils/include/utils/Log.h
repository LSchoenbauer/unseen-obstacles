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

#pragma once

#include <utils/ResultCode.h>
#include <stdarg.h>
#include <WString.h>

#ifndef LOG_ENABLED
#if defined(LOG_MSG_ENABLED) ||	\
	defined(LOG_DEBUG_ENABLED) || \
	defined(LOG_INFO_ENABLED) || \
	defined(LOG_WARN_ENABLED) || \
	defined(LOG_ERROR_ENABLED)
#define LOG_ENABLED
#endif
#endif

#ifdef LOG_ENABLED

#ifdef LOG_MSG_ENABLED
#define DO_ENABLE_LOG_MSG
#endif
#ifdef LOG_DEBUG_ENABLED
#define DO_ENABLE_LOG_DEBUG
#endif
#ifdef LOG_INFO_ENABLED
#define DO_ENABLE_LOG_INFO
#endif
#ifdef LOG_WARN_ENABLED
#define DO_ENABLE_LOG_WARN
#endif
#ifdef LOG_ERROR_ENABLED
#define DO_ENABLE_LOG_ERROR

#ifdef LOG_MULTITHREADED
	#define DO_LOG_MULTITHREADED
	#include <freertos/FreeRTOS.h>
	#include <freertos/task.h>
	#include <freertos/queue.h>
	#include <freertos/semphr.h>
#endif

#endif

#else /* LOG_ENABLED */

#endif /* LOG_ENABLED */

struct Logger {
	public:
		struct LogSink {
				typedef enum { // items must be bitwise distinct
					/** No log sink. */
					None = 0,
					/** log to serial */
					Serial = 1,
				} Enum;
		};

		/**
		 * This method initializes logging to the the specified log sinks.
		 * It must be called once during system initialization.
		 * 
		 * Important: This method is not thread safe even if LOG_MULTITHREADED is enabled!
		 * 
		 * @param sinksToEnable Bitwise OR of LogSink::Enum values to enable those sinks.
		 */
		static void Init(uint8_t sinksToEnable);

		static void LogLineBreak();
		static void Log(const char* level, const char* msg, ...);
		static void Log(const char* level, const char* file, const char* fct, int line, const char* msg, ...);
		static void LogLn(const char* level, const char* msg, ...);
		static void LogLn(const char* level, const char* file, const char* fct, int line, const char* msg, ...);
		static void LogLn(const char* level, const Rc& rc, const char* msg, ...);
		static void LogLn(const char* level, const char* file, const char* fct, int line, const Rc& rc, const char* msg, ...);
		static void LogLnIfFail(bool rc, const char* level, const char* msg, ...);
		static void LogLnIfFail(bool rc, const char* level, const char* file, const char* fct, int line, const char* msg, ...);
		static void LogLnIfFail(const Rc& rc, const char* level, const char* msg, ...);
		static void LogLnIfFail(const Rc& rc, const char* level, const char* file, const char* fct, int line, const char* msg, ...);
		
		/**
		 * Sets the file path prefix to remove for file paths in log messages.
		 * @param trimInclusive The file path prefix to remove in log messages.
		 */
		static void ShortenFilePath(const char* trimInclusive);

		/**
		 * Logs current heap statistics (free heap size and minimum free heap size).
		 */
		static void LogHeapStatistix();

		/**
		 * Converts the given boolean value as "YES" or "NO".
		 * @param value The boolean value to convert.
		 * @return "YES" for true, "NO" for false.
		 */
		static const char* Dump(bool value);
		/**
		 * Converts the given String object to a C string.
		 * @param value The String object to convert.
		 * @return The C string or "INV" if the String is invalid.
		 */
		static const char* Dump(const String& value);

	private:
		static uint8_t mEnabledSinks;
		static const char* mFilePathStarter;

		/**
		 * Enables or disables the specified log sink.
		 * @param sink The log sink to enable or disable.
		 * @param enable true to enable the sink, false to disable it.
		 */
		static void SetSinkEnabled(LogSink::Enum sink, bool enable);
		static bool IsSinkEnabled(LogSink::Enum sink);
		
		static const char* GetFileName(const char* file);
		static const char* GetFilePath(const char* file);

		static int CalcGap(int minLocatorLen, const char* file, const char* fct, int line);

		static void PrintLogMsg(const char* level, bool newLn, const char* file, const char* fct, int line, const Rc* rc,
		        const char* msg, va_list* args);
		static void DoLogMessage(const char* msg);
		static void ToSerial(const char* msg);

		static void CreateLoggingTask();
#ifdef DO_LOG_MULTITHREADED
		typedef struct {
			const char* msg;
		} LogEvent;

		static const uint32_t mLogBufferLength;
		static char mLogBuffer[];
		static size_t mLogBufferStartIdx;
		static size_t mNextFreeLogBufferIdx;

		static SemaphoreHandle_t mLogMutex;
		static QueueHandle_t mLogQueue;

		static void LoggingTask(void *pvParameters);
		static void EnqueueLog(const char* msg);
#endif

};

#define __LogLb "\r\n"
#define __LogLocatorMinLen__ 64

#ifdef LOG_ENABLED
#define IsLogEnabled() true
#define LogNewLine()   Logger::LogLineBreak()
#else /* LOG_ENABLED */
#define IsLogEnabled() false
#define LogNewLine()

#endif /* LOG_ENABLED */

#ifdef DBG

#define LogFormat(level, msg, ...)           Logger::LogLn(level, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define LogFormatStr(level, msg, ...)        Logger::Log(level, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define LogResult(level, msg, rc, ...)       Logger::LogLn(level, __FILE__, __FUNCTION__, __LINE__, rc, msg, ##__VA_ARGS__)
#define LogFailure(level, rc, msg, ...)      Logger::LogLnIfFail(rc, level, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
//#define LogFormat(level, format, ...)           Serial.printf("%s %s::%s #%d:%*s" format __LogLb, level, __LogFileName__, __FUNCTION__, __LINE__, __LogLocatorGap__, " ", ##__VA_ARGS__)
////#define LogFormat(level, msg, ...)             Serial.printf("%s %s::%s #%d:%*s" msg __LogLb, level, Logger::GetFileName(__FILE__), __FUNCTION__, __LINE__, Logger::CalcGap(__LogLocatorMinLen__, __FILE__, __FUNCTION__, __LINE__), " ", ##__VA_ARGS__)
//#define LogFormatStr(level, msg, ...)          Serial.printf("%s %s::%s #%d:%*s" msg, level, Logger::GetFileName(__FILE__), __FUNCTION__, __LINE__, Logger::CalcGap(__LogLocatorMinLen__, __FILE__, __FUNCTION__, __LINE__), " ", ##__VA_ARGS__)
//#define LogResult(level, msg, rc, ...)         Serial.printf("%s %s::%s #%d:%*s" msg " [#%d: %s]" __LogLb, level, Logger::GetFileName(__FILE__), __FUNCTION__, __LINE__, Logger::CalcGap(__LogLocatorMinLen__, __FILE__, __FUNCTION__, __LINE__), " ", ##__VA_ARGS__, rc.GetCode(), rc.GetName())
////#define LogFailure(level, rc, msg, ...)        if (!rc) { LogResult(level, msg, rc, ##__VA_ARGS__); }
//#define LogFailure(level, rc, msg, ...)      if (!rc) { LogFormat(level, msg, ##__VA_ARGS__); }

#else
#define LogFormat(level, msg, ...)           Logger::LogLn(level, msg, ##__VA_ARGS__)
#define LogFormatStr(level, msg, ...)        Logger::Log(level, msg, ##__VA_ARGS__)
#define LogResult(level, msg, rc, ...)       Logger::LogLn(level, rc, msg, ##__VA_ARGS__)
#define LogFailure(level, rc, msg, ...)      Logger::LogLnIfFail(rc, level, msg, ##__VA_ARGS__)
#endif

#ifdef DO_ENABLE_LOG_DEBUG

#define IsLogDbgEnabled() true
#define LogDbg(msg, ...)            LogFormat("D", msg, ##__VA_ARGS__)
#define LogDbgStr(msg, ...)         LogFormatStr("D", msg, ##__VA_ARGS__)
#define LogDbgRc(msg, rc, ...)      LogResult("D", msg, rc, ##__VA_ARGS__)
#define LogDbgFailure(rc, msg, ...) LogFailure("D", rc, msg, ##__VA_ARGS__)

#else

#define IsLogDbgEnabled() false
#define LogDbg(msg, ...)
#define LogDbgStr(msg, ...)
#define LogDbgRc(msg, rc, ...)
#define LogDbgFailure(rc, msg, ...)

#endif /* DO_ENABLE_LOG_DEBUG */

#ifdef DO_ENABLE_LOG_MSG

#define IsLogMsgEnabled() true
#define LogMsg(msg, ...)    Logger::LogLn(0, msg, ##__VA_ARGS__)
#define LogMsgStr(msg, ...) Logger::Log(0, msg, ##__VA_ARGS__)

#else

#define IsLogMsgEnabled() false
#define LogMsg(msg, ...)
#define LogMsgStr(msg, ...)

#endif /* DO_ENABLE_LOG_MSG */

#ifdef DO_ENABLE_LOG_INFO

#define IsLogInfoEnabled() true
#define LogInfo(msg, ...)            LogFormat("I", msg, ##__VA_ARGS__)
#define LogInfoStr(msg, ...)         LogFormatStr("I", msg, ##__VA_ARGS__)
#define LogInfoRc(msg, rc, ...)      LogResult("I", msg, rc, ##__VA_ARGS__)
#define LogInfoFailure(rc, msg, ...) LogFailure("I", rc, msg, ##__VA_ARGS__)

#else

#define IsLogInfoEnabled() false
#define LogInfo(msg, ...)
#define LogInfoStr(msg, ...)
#define LogInfoRc(msg, rc, ...)
#define LogInfoFailure(rc, msg, ...)

#endif /* DO_ENABLE_LOG_INFO */

#ifdef DO_ENABLE_LOG_WARN

#define IsLogWarnEnabled() true
#define LogWarn(msg, ...)            LogFormat("W", msg, ##__VA_ARGS__)
#define LogWarnStr(msg, ...)         LogFormatStr("W", msg, ##__VA_ARGS__)
#define LogWarnRc(msg, rc, ...)      LogResult("W", msg, rc, ##__VA_ARGS__)
#define LogWarnFailure(rc, msg, ...) LogFailure("W", rc, msg, ##__VA_ARGS__)

#else

#define IsLogWarnEnabled() false
#define LogWarn(msg, ...)
#define LogWarnStr(msg, ...)
#define LogWarnRc(msg, rc, ...)
#define LogWarnFailure(rc, msg, ...)

#endif /* DO_ENABLE_LOG_WARN */

#ifdef LOG_ERROR_ENABLED

#define IsLogErrorEnabled() true
#define LogError(msg, ...)            LogFormat("E", msg, ##__VA_ARGS__)
#define LogErrorStr(msg, ...)            LogFormatStr("E", msg, ##__VA_ARGS__)
#define LogErrorRc(msg, rc, ...)      LogResult("E", msg, rc, ##__VA_ARGS__)
#define LogErrorFailure(rc, msg, ...) LogFailure("E",rc, msg, ##__VA_ARGS__)

#else

#define IsLogErrorEnabled() false
#define LogError(msg, ...)
#define LogErrorStr(msg, ...)
#define LogErrorRc(msg, rc, ...)
#define LogErrorFailure(rc, msg, ...)

#endif /* LOG_ERROR_ENABLED */

#define DumpPtrStr(ptr, fct) (ptr != 0 ? fct : "NULL")
#define DumpPtrInt(ptr, fct) (ptr != 0 ? fct : 0)
#define DumpBool(boolVal) (boolVal ? 'Y' : 'N')
