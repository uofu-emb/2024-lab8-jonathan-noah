#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>

void canbus_setup(void);

int main( void )
{
    stdio_init_all();
    canbus_setup();
    for (;;) {
    }
    return 0;
}