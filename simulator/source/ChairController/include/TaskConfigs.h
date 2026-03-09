/**
 * Configuration for tasks, such as stack size and priority.
 * This file is included by all task implementations to ensure consistent configuration.
 */

#pragma once

#include <os/TaskMgmt.h>

using namespace Os;

/**
 * Configuration for tasks.
 */
class TaskConfigs {
    public:

        typedef enum  {
            CHAIR_CONTROLLER,
            UDP_SERVER,
            STEPPER_DRIVER,
            TASK_TABLE_DUMPER,
        } TaskCfgId;

        /**
         * Provides the configuration for the specified task.
         * @param taskCfgId The identifier of the taskfor which to retrieve the configuration.
         */
        static const TaskMgmt::TaskConfig& Get(TaskCfgId taskCfgId);

    private:
        TaskConfigs() = delete;
        ~TaskConfigs() = delete;

        static const TaskMgmt::TaskConfig mConfigs[];
};