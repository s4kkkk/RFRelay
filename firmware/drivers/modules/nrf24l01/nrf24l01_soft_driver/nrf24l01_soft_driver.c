/**
 * @file
 * @brief прослойка для интеграции драйвера nrf24l01_plus в проект
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <drivers/modules/nrf24l01/nrf24l01_soft_driver.h>

#include <stdint.h>

#include "drivers/modules/nrf24l01/nrf24l01.h"
#include "nrf24.h"

extern const struct device* nrf24l01_soft_driver_instance;

int8_t nrf24l01_soft_driver_init(const struct device* dev)
{
        struct nrf24l01_soft_driver_data* data = 
                (struct nrf24l01_soft_driver_data* ) dev->data;

        nrf24l01_soft_driver_instance = dev;

        nrf24_init();
        nrf24_config(data->channel, data->payload_len);


        return 0;
}

static int8_t nrf24l01_soft_driver_set_rx_addr(const struct device* dev, const uint8_t* addr)
{
        nrf24_rx_address( (uint8_t* ) addr);
        return 0;
}

static int8_t nrf24l01_soft_driver_set_tx_addr(const struct device* dev, const uint8_t* addr)
{
        nrf24_tx_address( (uint8_t* ) addr);
        return 0;
}

static int8_t nrf24l01_soft_driver_set_channel(const struct device* dev, uint8_t channel)
{
        if (! (channel >= 0 && channel <= 127) ) {
                return -1;
        }

        struct nrf24l01_soft_driver_data* data = 
                (struct nrf24l01_soft_driver_data* ) dev->data;

        nrf24_config(channel, data->payload_len);
        return 0;
}

static int8_t nrf24l01_soft_driver_set_payload_len(const struct device* dev,
                                                   uint8_t payload_len)
{
        if (! (payload_len > 0 && payload_len <= 32) ) {
                return -1;
        }

        struct nrf24l01_soft_driver_data* data = 
                (struct nrf24l01_soft_driver_data* ) dev->data;

        nrf24_config(data->channel, payload_len);
        return 0;
}

static int8_t nrf24l01_soft_driver_set_output_power(const struct device* dev,
                                                   enum nrf24l01_tx_power output_power)
{
        /* Не реализовано */
        return -1;
}

static int8_t nrf24l01_soft_driver_send(const struct device* dev, const uint8_t* buf)
{
        nrf24_send((uint8_t* )buf);
        return 0;
}

static int8_t nrf24l01_soft_driver_is_sending(const struct device* dev)
{
        return nrf24_isSending();
}

static int8_t nrf24l01_soft_driver_is_data_ready(const struct device* dev)
{
        return nrf24_dataReady();
}

static int8_t nrf24l01_soft_driver_get_data(const struct device* dev, uint8_t* buf)
{
        nrf24_getData(buf);
        return 0;
}

const struct nrf24l01_driver_api nrf24l01_soft_driver_api = {
        .nrf24l01_set_rx_addr = nrf24l01_soft_driver_set_rx_addr,
        .nrf24l01_set_tx_addr = nrf24l01_soft_driver_set_tx_addr,
        .nrf24l01_set_channel = nrf24l01_soft_driver_set_channel,
        .nrf24l01_set_payload_len = nrf24l01_soft_driver_set_payload_len,
        .nrf24l01_set_output_power = nrf24l01_soft_driver_set_output_power,
        .nrf24l01_send = nrf24l01_soft_driver_send,
        .nrf24l01_is_sending = nrf24l01_soft_driver_is_sending,
        .nrf24l01_is_data_ready = nrf24l01_soft_driver_is_data_ready,
        .nrf24l01_get_data = nrf24l01_soft_driver_get_data,
};
