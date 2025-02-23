#include <device.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <drivers/uart/uart_stm32f103cx.h>
#include <stdint.h>

#include "init.h"

static uint8_t rx_buf[32];

int main()
{
        init();
        int ret;

        /*
        const struct device* io_porta = device_get_by_name("gpio_0");

        while(1) {
                for(uint16_t i=0; i<1000; i++);
                gpio_pin_toggle(io_porta, 0);
        }
        */
        const struct device* uart1 = device_get_by_name("uart_1");
        ret = uart_set_rx_buffer(uart1, rx_buf, sizeof(rx_buf));
        if (ret != 0)
                while(1);

        ret = uart_rx_enable(uart1);
        if (ret != 0)
                while(1);

        uint8_t to_read_buf[32];
        size_t bytes_readed = 0;

        uart_tx_enable(uart1);
        /*
        while (1) {
                if (uart_is_data_available(uart1) && 
                    uart_is_transmitter_ready(uart1)) {
                        bytes_readed = uart_rx(uart1, to_read_buf, sizeof(to_read_buf));
                        uart_tx(uart1, to_read_buf, bytes_readed);
                }
                uart_stm32_driver_process(uart1);
        }
        */
        while (1) {
                while(!uart_is_data_available(uart1));
                bytes_readed = uart_rx(uart1, to_read_buf, sizeof(to_read_buf));
                while(!uart_is_transmitter_ready(uart1));
                uart_tx(uart1, to_read_buf, bytes_readed);
                uart_wait_for_tx_complete(uart1);
        }

        return 0;
}
