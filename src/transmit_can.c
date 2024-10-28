#include <can2040.h>
#include <stdio.h>
#include <pico/stdlib.h>

void canbus_setup(void);
int canbus_transmit(struct can2040_msg *msg);

int main( void )
{
    int mid = 0;
    struct can2040_msg msg;
    char msg_buf[256];
    stdio_init_all();
    canbus_setup();
    for (;;) {
        sprintf(msg_buf, "This is message #%d.", mid++);
        size_t n = strlen(msg_buf) + 1;
        msg.dlc = 8;
        // this may send a few extra bytes, but it's fine since the string is null-terminated
        for (size_t i = 0; i <= n / 8; i++) {
            memcpy(msg.data, msg_buf + 8*i, msg.dlc);
            while (canbus_transmit(&msg) < 0) sleep_ms(10);
        }
        sleep_ms(1000);
    }
    return 0;
}