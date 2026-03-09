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

#include <os/TaskMgmt.h>
#include <utils/Log.h>

namespace Os {

SemaphoreHandle_t TaskMgmt::mTaskCreationMutex = xSemaphoreCreateMutex();
TaskHandle_t TaskMgmt::mTaskListDumperTask = nullptr;
uint32_t TaskMgmt::mTaskListDumperPeriodMs = 0;

TaskHandle_t TaskMgmt::CreateTask(const TaskConfig& cfg, TaskFunction_t taskFn, void* taskParams, const char *taskName) {
    TaskHandle_t handle = nullptr;
    if (xSemaphoreTake(mTaskCreationMutex, pdMS_TO_TICKS(2)) == pdTRUE) {
        BaseType_t res = pdFAIL;
        const char* label = (taskName != nullptr) ? taskName : cfg.label;
        CoreId coreId = cfg.core;
#ifdef CONFIG_FREERTOS_UNICORE
        coreId = CoreId::CORE_ANY; // ignore requested core
#endif
        if (coreId == CoreId::CORE_ANY) {
            res = xTaskCreate(
                taskFn,            // Task function
                label,             // Task name
                cfg.stackDepth,    // Stacksize
                taskParams,        // Parameter
                cfg.priority,      // Priority
                &handle           // Task handle
            );
        } else {
            LogDbg("Creating task '%s' pinned to core %d with priority %d and stack depth %d",
                label, coreId, cfg.priority, cfg.stackDepth);
            res = xTaskCreatePinnedToCore(
                taskFn,            // Task function
                label,             // Task name
                cfg.stackDepth,    // Stacksize
                taskParams,        // Parameter
                cfg.priority,      // Priority
                &handle,           // Task handle
                coreId             // pin to core
            );
        }
        if (res != pdPASS) {
            LogError("Failed creating task '%s'", label);
            handle = nullptr;
        } else {
            if (coreId  == CoreId::CORE_ANY) {
                LogDbg("Created task '%s' (hdl: %p) on ANY core with priority %d and stack depth %d",
                    label, handle, cfg.priority, cfg.stackDepth);
            }
            else {
                LogDbg("Created task '%s' (hdl: %p) pinned to core %d with priority %d and stack depth %d",
                    label, handle, coreId, cfg.priority, cfg.stackDepth);
            }
            vTaskDelay(1);
            if (eTaskGetState(handle) == eDeleted) {
                LogError("Task '%s' was deleted immediately!", label);
                handle = nullptr;
            }
        }
        xSemaphoreGive(mTaskCreationMutex);
    } else {
        LogError("Task creation failed: Mutex could not be acquired for task '%s'", cfg.label);
    }
    return handle;
}

void TaskMgmt::PrintTaskList() {
    UBaseType_t taskCount = uxTaskGetNumberOfTasks();
    TaskStatus_t* taskStates = (TaskStatus_t*)pvPortMalloc(taskCount * sizeof(TaskStatus_t));
    uint32_t totalTime = 0;
    double totalRunTimeBase = 0;
    double taskRunTimeMs = 0;
    double taskRunTimePc = 0;

    if (taskStates != nullptr) {
        taskCount = uxTaskGetSystemState(taskStates, taskCount, &totalTime);

        LogInfo("\t==================================================================================================");
        LogInfo("\t=== FreeRTOS Detailed Task Info ===");
        LogInfo("\t\tTotal Runtime: %u µs", totalTime);
        LogInfo("\tID\tState\tTask Name\t\tPrio\tCore\tTime [ms]\tTime [%%]\tStack Used");
        LogInfo("\t--------------------------------------------------------------------------------------------------");
        totalRunTimeBase = totalTime / 100.0;
        for (UBaseType_t i = 0; i < taskCount; i++) {
            taskRunTimeMs = taskStates[i].ulRunTimeCounter / 1000.0;
            taskRunTimePc = (totalRunTimeBase > 0.0) ? taskStates[i].ulRunTimeCounter / totalRunTimeBase : 0.0;
            size_t tabLen = strlen(taskStates[i].pcTaskName) / 8; // 8: tab size
            LogInfo(
                "\t%u\t%s\t%s\t%s%3d\t  %s\t%11.3f\t  %s%5.1f\t%10u"
                , taskStates[i].xTaskNumber
                , GetStateSign(taskStates[i].eCurrentState)
                , taskStates[i].pcTaskName
                ,(tabLen > 1) ? "" : (tabLen == 1) ? "\t" : "\t\t"
                , taskStates[i].uxCurrentPriority
#ifdef CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID
                , taskStates[i].xCoreID == 0 ? "0" : taskStates[i].xCoreID == 1 ? "1" : "x"
#else
                , "?" // core ID not available
#endif
                , taskRunTimeMs
                , taskRunTimePc < 1.0 ? "<" : " "
                , taskRunTimePc < 1.0 ? 1.0 : taskRunTimePc
                , taskStates[i].usStackHighWaterMark
            );
        }
        LogInfo("\t--------------------------------------------------------------------------------------------------");
        vPortFree(taskStates);
    }
}

void TaskMgmt::StartTaskListDumper(uint32_t periodMs) {
    TaskConfig cfg = { 
        1234,
        "top",
        TaskMgmt::CoreId::CORE_ANY,
        4096,
        3
    };
    StartTaskListDumper(cfg, periodMs);
}

void TaskMgmt::StartTaskListDumper(const TaskConfig& taskCfg, uint32_t periodMs) {
    if (xSemaphoreTake(mTaskCreationMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
        mTaskListDumperPeriodMs = periodMs;
        xSemaphoreGive(mTaskCreationMutex);
        if (mTaskListDumperTask == nullptr) {
            mTaskListDumperTask = CreateTask(taskCfg, RunTaskListDumper, nullptr);
        }
    }
}

void TaskMgmt::StopTaskListDumper() {
    if (xSemaphoreTake(mTaskCreationMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
        LogDbg("TaskMgmt: Stopping task list dumping task");
        mTaskListDumperPeriodMs = 0; // signal the dumper task to stop
        xSemaphoreGive(mTaskCreationMutex);
    }
}

void TaskMgmt::RunTaskListDumper(void* params) {
    while (mTaskListDumperPeriodMs > 0) {
        PrintTaskTable();
        vTaskDelay(pdMS_TO_TICKS(mTaskListDumperPeriodMs)); 
    }
    vTaskDelete(mTaskListDumperTask);
    mTaskListDumperTask = nullptr;
    LogDbg("TaskMgmt: Stopped task list dumping");
}

const char* TaskMgmt::GetStateName(eTaskState state) {
    switch (state) {
        case eRunning:   return "running";
        case eReady:     return "ready";
        case eBlocked:   return "blocked";
        case eSuspended: return "suspended";
        case eDeleted:   return "deleted";
        case eInvalid:   return "invalid";
    }
    return "unknown";
}

const char* TaskMgmt::GetStateSign(eTaskState state) {
    switch (state) {
        case eRunning:   return "X";
        case eReady:     return "R";
        case eBlocked:   return "B";
        case eSuspended: return "S";
        case eDeleted:   return "D";
        case eInvalid:   return "I";
    }
    return "?";
}

} // Os