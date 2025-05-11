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
const char* msg = "OK\n";

enum {
        TX_SENDING,
        TX_WAIT_FOR_SEND_COMPLETE,
} tx_state = TX_SENDING;

static inline void process_transceiver()
{
        switch (tx_state) {
                case TX_SENDING: {
                        nrf24l01_send(nrf24l01_dev, (const uint8_t* ) msg);
                        tx_state = TX_WAIT_FOR_SEND_COMPLETE;
                        break;
                }

                case TX_WAIT_FOR_SEND_COMPLETE: {
                        if (!nrf24l01_is_sending(nrf24l01_dev)) {
                                DEBUG("Sending complete!\n");
                                tx_state  = TX_SENDING;
                        }
                        else {
                                DEBUG("Waiting for send complete!\n");
                        }
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
