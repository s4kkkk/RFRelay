#include <device.h>
#include <printk.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <drivers/uart/uart_stm32f103cx.h>
#include <stdint.h>

#include "init.h"

int main()
{
        init();

        while (1) {
                for (uint32_t i = 0; i < 100000; i++) {
                        printk("Iteration: %d\n", i);
                }
        }

        return 0;
}
