#include <device.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include "init.h"

int main()
{
        init();

        const struct device* io_porta = device_get_by_name("gpio_0");

        while(1) {
                for(uint16_t i=0; i<1000; i++);
                gpio_pin_toggle(io_porta, 0);
        }

        return 0;
}
