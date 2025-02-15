/**
 * @file 
 * @brief Реализация GPIO-драйвера для stm32f103c8
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <stm32f1xx.h>

#include <device.h>
#include <drivers_init/gpio_stm32_init.h>

static const struct gpio_stm32_settings gpioa_settings = {
        .port = 0
};
DEVICE_GPIO_STM32_DEFINE(gpio_01, &gpioa_settings);

int init_stage0()
{
        gpio_stm32_init_driver(&gpio_01);
        device_register(&gpio_01);
        return 0;
}
