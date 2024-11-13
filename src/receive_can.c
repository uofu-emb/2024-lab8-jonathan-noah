/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <can2040.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

void canbus_setup(void);

QueueHandle_t message_queue;

void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    xQueueSendToBack(message_queue, msg, 20);
}

void main_task(__unused void *params) {
    struct can2040_msg msg;
    char msg_buf[256];
    int idx = 0;

    message_queue = xQueueCreate(100, sizeof(struct can2040_msg));

    for (;;) {
        if (xQueueReceive(message_queue, &msg, portMAX_DELAY) != pdTRUE) continue;
        memcpy(msg.data, &msg_buf[idx], msg.dlc);
        if (strlen(&msg_buf[idx]) < 8) {
            printf("%s\n", msg_buf);
            idx = 0;
        }
        else idx += msg.dlc;
    }
}

int main( void )
{
    stdio_init_all();
    canbus_setup();
    const char *rtos_name;
    rtos_name = "FreeRTOS";
    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &task);
    vTaskStartScheduler();
    return 0;
}
