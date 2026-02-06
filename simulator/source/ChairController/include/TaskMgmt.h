/**
 * Configuration for tasks, such as stack size and priority.
 * This file is included by all task implementations to ensure consistent configuration.
 */

#pragma once
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>

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
         * @param taskId The identifier of the task to create and start.
         * @param taskFn The function that implements the task.
         * @param taskParams Optional parameters to pass to the task function.
         * @param taskName Optional name for the task. If nullptr, the label from the configuration is used.
         * @return The handle of the created task or nullptr if the task could not be created.
         */
        static TaskHandle_t CreateTask(TaskId taskId, TaskFunction_t taskFn, void* taskParams = nullptr, const char *taskName = nullptr);

    private:
        TaskMgmt() = delete;
        ~TaskMgmt() = delete;

        static const TaskConfig mConfigs[];
};