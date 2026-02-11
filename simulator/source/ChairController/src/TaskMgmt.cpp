#include "TaskMgmt.h"
#include <utils/Log.h>

const TaskMgmt::TaskConfig TaskMgmt::mConfigs[] = {
    // index corresponds to 'Task' enum value!
    // ID, label, core, stackDepth, priority (0 (idle) to configMAX_PRIORITIES-1 (max))
    { TaskMgmt::TaskId::CHAIR_CONTROLLER, "ChairController", TaskMgmt::CoreId::CORE_1, 4096, configMAX_PRIORITIES - 5 },
    { TaskMgmt::TaskId::UDP_SERVER,       "UdpServer",       TaskMgmt::CoreId::CORE_1, 2048, configMAX_PRIORITIES - 8 },
    { TaskMgmt::TaskId::STEPPER_DRIVER,   "StepperDriver",   TaskMgmt::CoreId::CORE_0, 4096, configMAX_PRIORITIES - 3 },
};

TaskMgmt::TaskConfig TaskMgmt::GetConfig(TaskId taskId) {
    // configurations are defined in TaskMgmt.cpp
    TaskConfig cfg = mConfigs[static_cast<uint32_t>(taskId)];
    if (cfg.task != taskId) {
        LogError("Invalid task configuration for requested task %d:", taskId);
        LogError("  -> received configuration for task %d!", cfg.task);
        LogError("  Check task configuration in TaskMgmt.cpp");
        LogError("  Device will restart!");
        vTaskDelay(pdMS_TO_TICKS(500)); // give some time for the log to be sent before restarting
        esp_restart();
    }
    return cfg;
}

TaskHandle_t TaskMgmt::CreateTask(TaskId taskId, TaskFunction_t taskFn, void* taskParams, const char *taskName) {
    BaseType_t res = pdFAIL;
    TaskHandle_t handle = nullptr;
    TaskConfig cfg = GetConfig(taskId);
    const char* label = (taskName != nullptr) ? taskName : cfg.label;
    if (cfg.core == CoreId::CORE_ANY) {
        res = xTaskCreate(
            taskFn,            // Task function
            label,             // Task name
            cfg.stackDepth,    // Stacksize
            taskParams,        // Parameter
            cfg.priority,      // Priority
            &handle           // Task handle
        );
    } else {
        res = xTaskCreatePinnedToCore(
            taskFn,            // Task function
            label,             // Task name
            cfg.stackDepth,    // Stacksize
            taskParams,        // Parameter
            cfg.priority,      // Priority
            &handle,          // Task handle
            cfg.core           // pin to core 0
        );
    }
    if (res != pdPASS) {
        LogError("Failed creating task %s (ID %d)", label, taskId);
    } else {
        if (cfg.core  == CoreId::CORE_ANY) {
            LogDbg("Created task %s (ID %d) on ANY core with priority %d and stack depth %d",
                label, taskId, cfg.priority, cfg.stackDepth);
        }
        else {
            LogDbg("Created task %s (ID %d, hdl: %p) pinned to core %d with priority %d and stack depth %d",
                label, taskId, handle, cfg.core, cfg.priority, cfg.stackDepth);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return (res == pdPASS) ? handle : nullptr;
}

void TaskMgmt::TraceTaskList()
{
    // char buffer[2048];
    // vTaskList(buffer);
    // LogDbg("Name          State  Prio Stack Num\n");
    // LogDbg("%s\n", buffer);
}

void TaskMgmt::TraceTaskStats()
{
    // char buffer[2048];
    // vTaskGetRunTimeStats(buffer);
    // LogDbg("Task           Time      Percent\n");
    // LogDbg("%s\n", buffer);
}

void TaskMgmt::TraceTaskDetails() {
    // UBaseType_t taskCount = uxTaskGetNumberOfTasks();
    // TaskStatus_t* statusArray = (TaskStatus_t*)malloc(taskCount * sizeof(TaskStatus_t));
    // uint32_t totalRunTime;

    // taskCount = uxTaskGetSystemState(
    //     statusArray,
    //     taskCount,
    //     &totalRunTime
    // );

    // for (int i = 0; i < taskCount; i++) {
    //     printf("%s: %lu\n",
    //         statusArray[i].pcTaskName,
    //         statusArray[i].ulRunTimeCounter);
    // }

    // free(statusArray);
}
