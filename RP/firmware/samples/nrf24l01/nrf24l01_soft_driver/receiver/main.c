#include <printk.h>
#include <drivers/gpio/gpio.h>
#include <drivers/uart/uart.h>
#include <drivers/modules/nrf24l01/nrf24l01.h>

#include <stdint.h>

#include "device.h"
#include "init.h"

#define PANIC() while(1)

#define DEBUG(x) printk(x)
// #define DEBUG(x)

const struct device* nrf24l01_dev;
uint8_t buf[4];

enum {
        RX_DATA_WAIT,
        RX_DATA_COPYING,
} rx_state = RX_DATA_WAIT;

static inline void process_transceiver()
{
        switch (rx_state) {
                case RX_DATA_WAIT: {
                        if (nrf24l01_is_data_ready(nrf24l01_dev)) {
                                rx_state = RX_DATA_COPYING;
                                printk("Data is received\n");
                        }
                        break;
                }
                
                case RX_DATA_COPYING: {
                        nrf24l01_get_data(nrf24l01_dev, buf);
                        buf[3] = 0;
                        printk("Data: ");
                        printk((char* ) buf);

                        rx_state = RX_DATA_WAIT;
                        break;
                }
        }

        return;
}

int main()
{
        if (init() != 0) {
                printk("Init failed! Panic!\n");
                PANIC();
        }

        printk("Init success!\n");

        nrf24l01_dev = device_get_by_name("nrf24l01_1");
        if (nrf24l01_dev == NULL) {
                printk("Error in founding nrf24l01 device!\n");
                PANIC();
        }

        while (1) {
                process_transceiver();
        }

        return 0;
}
