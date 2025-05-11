/**
 * @file 
 * @brief Реализация начальной инициализации
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "init.h"

#include "device.h"
#include <printk.h>
#include "drivers/modules/nrf24l01/nrf24l01.h"
#include "drivers/uart/uart.h"
#include <drivers/gpio/gpio_stm32f103cx.h>
#include <drivers/uart/uart_stm32f103cx.h>
#include <drivers/modules/nrf24l01/nrf24l01_soft_driver.h>

/* Создание экземпляров драйверов */ 
static const struct uart_stm32_settings uart1_settings = {
        .uart_controller_num = 1,
        .uart_controller_clk = 8000000
};
DEVICE_UART_STM32_DEFINE(uart_1, &uart1_settings);

static const struct gpio_stm32_settings gpio_a_settings = {
        .port = 0,
};
DEVICE_GPIO_STM32_DEFINE(gpio_a, &gpio_a_settings);

static const struct nrf24l01_soft_driver_settings nrf24l01_settings = {
        .gpio_controller = &gpio_a,
        .pins = {
                .ce_pin = 4,
                .cs_pin = 5,
                .sck_pin = 6,
                .mosi_pin = 7,
                .miso_pin = 3
        },
};
DEVICE_NRF24L01_SOFT_DRIVER_DEFINE(nrf24l01_1, &nrf24l01_settings);


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

        ret = gpio_stm32_init_driver(&gpio_a);
        if (ret != 0)
                return -1;
        ret = device_register(&gpio_a);
        if (ret != 0)
                return -1;

        ret = nrf24l01_soft_driver_init(&nrf24l01_1);
        if (ret != 0)
                return -1;
        ret = device_register(&nrf24l01_1);
        if (ret != 0)
                return -1;

        return 0;
}

static inline int init_stage1()
{
        int ret;

        const struct device* dev = device_get_by_name("uart_1");
        if (dev == NULL)
                return -1;

        struct uart_config uart_cfg = {
                .baudrate = 9600,
                .character_bits = UART_CONFIG_CHARACTER_BITS_8,
                .stop_bits = UART_CONFIG_STOP_BITS_1
        };

        ret = uart_configure(dev, &uart_cfg);
        if (ret != 0)
                return -1;
        uart_tx_enable(dev);
        set_printk_controller(dev);

        dev = device_get_by_name("nrf24l01_1");
        if (dev == NULL)
                return -1;

        /* второй канал */
        ret = nrf24l01_set_channel(dev, 2);
        if (ret != 0)
                return -1;

        /* полезная нагрузка - 3 байта*/
        ret = nrf24l01_set_payload_len(dev, 3);
        if (ret != 0)
                return -1;

        uint8_t rx_mac_addr[5] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};
        uint8_t tx_mac_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};


        ret = nrf24l01_set_rx_addr(dev, rx_mac_addr);
        if (ret != 0)
                return -1;
        ret = nrf24l01_set_tx_addr(dev, tx_mac_addr);
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
