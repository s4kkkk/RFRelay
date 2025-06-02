#include <device.h>
#include <printk.h>
#include <drivers/gpio/gpio.h>
#include <drivers/uart/uart.h>
#include <drivers/uart/uart_stm32f103cx.h>
#include <drivers/modules/nrf24l01/nrf24l01.h>

#include "config.h"
#include "init.h"
#include "timer.h"
#include "packets.h"

int main()
{
        DEBUG("Initializating started...\n");

        int ret;

        ret = init();
        if (ret != 0) {
                DEBUG("Panic: hardware initializating failure\n");
                while(1);
        }

        struct timer_t test_timer;
        setup_timer(&test_timer, 5000);

        reset_timer(&test_timer);
        while (1) {
                if (is_timer_elapsed(&test_timer)) {
                        printk("Прошло 5 секунд\n");
                        reset_timer(&test_timer);
                }
        }

        return 0;
}
