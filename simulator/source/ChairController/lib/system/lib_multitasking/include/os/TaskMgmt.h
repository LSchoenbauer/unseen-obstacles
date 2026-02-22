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

#pragma once

#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

namespace Os {

// Allows enabling or disabling the printing of task information for debugging purposes.
// This are production ready methods and can be left enabled in production code, but they require additional FreeRTOS configuration options to be enabled (see method documentation for details).
#ifdef TASK_INFO_ENABLED
    #ifndef CONFIG_FREERTOS_USE_TRACE_FACILITY
        #error "To enable task information printing, the FreeRTOS configuration option CONFIG_FREERTOS_USE_TRACE_FACILITY must be enabled."
    #endif
    #ifndef CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
        #warning "To support run time information in task table, the FreeRTOS configuration option CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS must be enabled."
    #endif
    #ifndef CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID
        #warning "To support core details in task table, the FreeRTOS configuration option CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID must be enabled."
    #endif
    #define PrintTaskTable() TaskMgmt::PrintTaskList()
    #define StartTop(periodMs) TaskMgmt::StartTaskListDumper(periodMs)
    #define StartTopInTask(taskCfg, periodMs) TaskMgmt::StartTaskListDumper(taskCfg, periodMs)
    #define StopTop() TaskMgmt::StopTaskListDumper()
#else
    #define PrintTaskTable()
    #define StartTop(periodMs)
    #define StartTopInTask(taskCfg, periodMs)
    #define StopTop()
#endif

/**
 * Configuration for tasks.
 */
class TaskMgmt {
    public:

        typedef enum {
            CORE_0 = 0,
            CORE_1 = 1,
            CORE_ANY
        } CoreId;
        
        /**
         * Configuration for a task.
         */
        typedef struct {
            uint32_t cfgId;          // identifier for the task configuration, used for logging and debugging
            const char* label;       // human-readable label for the task, used in logging
            CoreId core;             // CPU core to which the task is pinned
            uint32_t stackDepth;     // depth of the call stack in words
            UBaseType_t priority;    // FreeRTOS priority
        } TaskConfig;

        /**
         * Creates and starts a task with the specified configuration.
         * Created tasks may be deleted using vTaskDelete() with the returned task handle.
         * @param cfg The task configuration to use for creating the task.
         * @param taskFn The function that implements the task.
         * @param taskParams Optional parameters to pass to the task function.
         * @param taskName Optional name for the task. If nullptr, the label from the configuration is used.
         * @return The handle of the created task or nullptr if the task could not be created.
         */
        static TaskHandle_t CreateTask(const TaskConfig& cfg, TaskFunction_t taskFn, void* taskParams = nullptr, const char *taskName = nullptr);

        /**
         * Provides the human-readable name of the specified task state.
         * @param state The task state for which to retrieve the name.
         * @return The name of the task state as a string.
         */
        static const char* GetStateName(eTaskState state);

        /**
         * Use PrintTaskTable() instead to enabled or disable this method 
         * via compile switch TASK_STATS_ENABLED.
         * 
         * Prints detailed information about the currently running tasks to the log, 
         * including their state, stack usage, and CPU core assignment.
         * Useful for debugging task scheduling and performance issues.
         * 
         * @note: The method requires the following compiler options to be enabled:
         *    TASK_INFO_ENABLED
         * @note: The following FreeRTOS configuration options must be enabled:
         *    CONFIG_FREERTOS_USE_TRACE_FACILITY
         *    CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS (for run time information)
         *    CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID (for core assignment information)
         */
        static void PrintTaskList();

        /**
         * Prints the task list periodically with the specified period in milliseconds.
         * Useful for monitoring task states and performance over time.
         * Subsequent call can be used to adapt the period of printing the task list.
         * 
         * @note: The compiler and FreeRTOS configuration options as for PrintTaskList must be enabled!
         * 
         * @param periodMs The period in milliseconds between each print of the task list.
         */
        static void StartTaskListDumper(uint32_t periodMs);

        /**
         * Prints the task list periodically with the specified period in milliseconds.
         * Useful for monitoring task states and performance over time.
         * Subsequent call can be used to adapt the period of printing the task list.
         * 
         * @note: The compiler and FreeRTOS configuration options as for PrintTaskList must be enabled!
         * 
         * @param taskCfg The configuration to use for the task that prints the task list.
         * @param periodMs The period in milliseconds between each print of the task list.
         */
        static void StartTaskListDumper(const TaskConfig& taskCfg, uint32_t periodMs);

        /**
         * Stops the periodic printing of the task list started by StartTaskListDumper.
         */
        static void StopTaskListDumper();

    private:
        TaskMgmt() = delete;
        ~TaskMgmt() = delete;

        static const char* GetStateSign(eTaskState state);
        static void RunTaskListDumper(void* params);

        // the mutex to protect task creation
        static SemaphoreHandle_t mTaskCreationMutex;
        static TaskHandle_t mTaskListDumperTask;
        static uint32_t mTaskListDumperPeriodMs;

};

} // Os