#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static struct can2040 cbus;

QueueHandle_t message_queue = NULL;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (message_queue) {
        xQueueSendToBackFromISR(message_queue, msg, NULL);
    }
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

static void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

void transmit_task(__unused void *params) {
    int mid = 0;
    struct can2040_msg msg;
    char msg_buf[256];

    for (;;) {
        sprintf(msg_buf, "This is message #%d.", mid++);
        size_t n = strlen(msg_buf) + 1;
        msg.dlc = 8;
        // this may send a few extra bytes, but it's fine since the string is null-terminated
        for (size_t i = 0; i <= n / 8; i++) {
            memcpy(msg.data, msg_buf + 8*i, msg.dlc);
            while (can2040_transmit(&cbus, &msg) < 0) sleep_ms(10);
        }
        vTaskDelay(1000);
    }
}

void receive_task(__unused void *params) {
    struct can2040_msg msg;

    message_queue = xQueueCreate(100, sizeof(struct can2040_msg));

    for (;;) {
        if (xQueueReceive(message_queue, &msg, portMAX_DELAY) != pdTRUE) continue;
        char buf[9] = {0};
        memcpy(msg.data, buf, msg.dlc);
        printf(buf);
        if (strlen(buf) < msg.dlc) printf("\n");
    }
}

int main( void )
{
    stdio_init_all();
    canbus_setup();
    const char *rtos_name;
    rtos_name = "FreeRTOS";
    TaskHandle_t rtask, ttask;
    xTaskCreate(transmit_task, "TransmitThread",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &ttask);
    xTaskCreate(receive_task, "ReceiveThread",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2UL, &rtask);
    vTaskStartScheduler();
    return 0;
}