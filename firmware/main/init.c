/**
 * @file 
 * @brief Реализация начальной инициализации
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "init.h"
#include "device.h"
#include "drivers/gpio.h"
#include <drivers_init/gpio_stm32_init.h>

static const struct gpio_stm32_settings gpioa_settings = {
        .port = 0
};
DEVICE_GPIO_STM32_DEFINE(gpio_0, &gpioa_settings);

static inline int init_clock()
{
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        return 0;
}

static inline int init_stage0()
{
        int ret;

        ret = init_clock();
        if (ret != 0)
                return -1;

        ret = gpio_stm32_init_driver(&gpio_0);
        if (ret != 0)
                return -1;

        ret = device_register(&gpio_0);
        if (ret != 0)
                return -1;

        return 0;
}

static inline int init_stage1()
{
        int ret;

        const struct device* io_porta = device_get_by_name("gpio_01");
        if (io_porta == NULL)
                return -1;

        ret = gpio_pin_configure(io_porta, 0, GPIO_OUTPUT);
        if (ret != 0)
                return -1;

        return 0;
}

int init()
{
        int ret;
        
        ret = init_stage0();
        if (ret != 0)
                return -1;

        ret = init_stage1();
        if (ret != 0)
                return -1;

        return 0;
}
