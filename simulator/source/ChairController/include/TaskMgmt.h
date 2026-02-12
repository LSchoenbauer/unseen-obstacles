/**
 * Configuration for tasks, such as stack size and priority.
 * This file is included by all task implementations to ensure consistent configuration.
 */

#pragma once
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>

// Allows enabling or disabling the printing of task information for debugging purposes.
// This are production ready methods and can be left enabled in production code, but they require additional FreeRTOS configuration options to be enabled (see method documentation for details).
#ifdef TASK_INFO_ENABLED
    #define PrintTasks() TaskMgmt::PrintTaskList()
    #define PrintTasksDetailed() TaskMgmt::PrintDetailedTaskList()
    #define PrintTaskRuntimeDetails() TaskMgmt::PrintTaskRuntimeStatistics()
#else
    #define PrintTasks()
    #define PrintTasksDetailed()
    #define PrintTaskRuntimeDetails()
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
         * Enumerates the system tasks.
         */
        typedef enum  {
            CHAIR_CONTROLLER,
            UDP_SERVER,
            STEPPER_DRIVER,
        } TaskId;

        /**
         * Configuration for a task.
         */
        typedef struct {
            TaskId task;
            const char* label;
            CoreId core;         // CPU core to which the task is pinned
            uint32_t stackDepth;     // depth of the call stack in words
            UBaseType_t priority;    // FreeRTOS priority
        } TaskConfig;

        /**
         * Provides the configuration for the specified task.
         * @param taskId The identifier of the taskfor which to retrieve the configuration.
         */
        static TaskConfig GetConfig(TaskId taskId);
        /**
         * Creates and starts a task with the specified configuration.
         * Created tasks may be deleted using vTaskDelete() with the returned task handle.
         * @param taskId The identifier of the task to create and start.
         * @param taskFn The function that implements the task.
         * @param taskParams Optional parameters to pass to the task function.
         * @param taskName Optional name for the task. If nullptr, the label from the configuration is used.
         * @return The handle of the created task or nullptr if the task could not be created.
         */
        static TaskHandle_t CreateTask(TaskId taskId, TaskFunction_t taskFn, void* taskParams = nullptr, const char *taskName = nullptr);
        static void TraceTaskList();
        static void TraceTaskStats();
        static void TraceTaskDetails();

        /**
         * Use PrintTasks() instead to enabled or disable this method 
         * via compile switch TASK_STATS_ENABLED.
         * 
         * Prints a list of all currently running tasks to the log.
         * Useful for debugging task creation and scheduling issues.
         * 
         * Interrupts are disabled while retrieving the task list.
         * This method shall not be used in production code.
         * 
         * @note: The following FreeRTOS configuration option must be enabled:
         *    CONFIG_FREERTOS_USE_TRACE_FACILITY
         *    CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
         */
        static void PrintTaskList();

        /**
         * Use PrintTasksDetailed() instead to enabled or disable this method 
         * via compile switch TASK_STATS_ENABLED.
         * 
         * Prints detailed information about the currently running tasks to the log, 
         * including their state, stack usage, and CPU core assignment.
         * Useful for debugging task scheduling and performance issues.
         * 
         *@note: The following FreeRTOS configuration options must be enabled:
         *    CONFIG_FREERTOS_USE_TRACE_FACILITY
         *    CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID (for core assignment information)
         */
        static void PrintDetailedTaskList();

        /**
         * Use PrintTaskRuntimeDetails() instead to enabled or disable this method 
         * via compile switch TASK_STATS_ENABLED.
         * 
         * Prints runtime statistics for the currently running tasks to the log, 
         * including their CPU usage percentage.
         * Useful for performance analysis and optimization.
         * 
         * @note: The following FreeRTOS configuration options must be enabled:
         *    CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
         *    CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
         */
        static void PrintTaskRuntimeStatistics();

    private:
        TaskMgmt() = delete;
        ~TaskMgmt() = delete;

        static const TaskConfig mConfigs[];
};