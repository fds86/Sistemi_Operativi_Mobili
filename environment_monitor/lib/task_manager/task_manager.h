#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

/**
 * @file task_manager.h
 * @brief Public API to initialize and start project FreeRTOS tasks.
 * @details See ../docs/freertos_task_manager.md for the task architecture,
 *          synchronization model, and the FreeRTOS APIs used by the module.
 */

/**
 * @brief Initializes modules and creates sensor, actuator, and MQTT tasks.
 */
void TaskManager_SetupTaskManager();

#endif