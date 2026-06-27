#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

/**
 * @file task_manager.h
 * @brief Public API to initialize and start project FreeRTOS tasks.
 */

/**
 * @brief Initializes modules and creates sensor/control/MQTT tasks.
 */
void TaskManager_SetupTaskManager();

#endif