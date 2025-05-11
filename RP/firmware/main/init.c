/**
 * @file 
 * @brief Реализация начальной инициализации
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "init.h"

#include "device.h"
#include <printk.h>
#include "drivers/gpio.h"
#include "drivers/uart.h"
#include <drivers/gpio/gpio_stm32f103cx.h>
#include <drivers/uart/uart_stm32f103cx.h>

static const struct uart_stm32_settings uart1_settings = {
        .uart_controller_num = 1,
        .uart_controller_clk = 8000000
};
DEVICE_UART_STM32_DEFINE(uart_1, &uart1_settings);

static inline int init_clock()
{
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        return 0;
}

static inline int init_stage0()
{
        int ret;

        ret = init_clock();
        if (ret != 0)
                return -1;

        ret = uart_stm32_init_driver(&uart_1);
        if (ret != 0)
                return -1;

        ret = device_register(&uart_1);
        if (ret != 0)
                return -1;

        return 0;
}

static inline int init_stage1()
{
        int ret;

        const struct device* uart_1 = device_get_by_name("uart_1");
        if (uart_1 == NULL)
                return -1;

        struct uart_config uart_cfg = {
                .baudrate = 9600,
                .character_bits = UART_CONFIG_CHARACTER_BITS_8,
                .stop_bits = UART_CONFIG_STOP_BITS_1
        };

        ret = uart_configure(uart_1, &uart_cfg);
        if (ret != 0)
                return -1;
        uart_tx_enable(uart_1);

        set_printk_controller(uart_1);

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
