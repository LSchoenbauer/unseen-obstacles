#include "TaskConfigs.h"
#include <utils/Log.h>

const TaskMgmt::TaskConfig TaskConfigs::mConfigs[] = {
    // index corresponds to 'Task' enum value!
    // id, label, core, stackDepth, priority (0 (idle) to configMAX_PRIORITIES-1 (max))
    { CHAIR_CONTROLLER,  "chair_ctrl",  TaskMgmt::CoreId::CORE_1,   8196, configMAX_PRIORITIES - 10 },
    { UDP_SERVER,        "udp_server",  TaskMgmt::CoreId::CORE_1,   4096, configMAX_PRIORITIES - 13 },
    { STEPPER_DRIVER,    "stepper_drv", TaskMgmt::CoreId::CORE_0,   6144, configMAX_PRIORITIES -  5 },
    { TASK_TABLE_DUMPER, "task_tbl",    TaskMgmt::CoreId::CORE_ANY, 4096,                         3 },
};

const TaskMgmt::TaskConfig& TaskConfigs::Get(TaskCfgId cfgId) {
    const TaskMgmt::TaskConfig& cfg = mConfigs[static_cast<uint32_t>(cfgId)];
    if (cfg.cfgId != cfgId) {
        LogError("Invalid task configuration for requested task %d:", cfgId);
        LogError("  -> received configuration for task %d!", cfg.cfgId);
        LogError("  Check task configuration in TaskMgmt.cpp");
        LogError("  Device will restart!");
        vTaskDelay(pdMS_TO_TICKS(500)); // give some time for the log to be sent before restarting
        esp_restart();
    }
    return cfg;
}
