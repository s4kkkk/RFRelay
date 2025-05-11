/**
 * @file 
 * @brief Интерфейс к модулю nrf24l01
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INCLUDE_DRIVERS_MODULES_NRF24L01_H
#define INCLUDE_DRIVERS_MODULES_NRF24L01_H

#include <device.h>

#include <stdint.h>
#include <stddef.h>

/**
 * @brief мощность передатчика
 */
enum nrf24l01_tx_power {
        NRF24L01_OUTPUT_POWER_NEG_18_DBM,
        NRF24L01_OUTPUT_POWER_NEG_12_DBM,
        NRF24L01_OUTPUT_POWER_NEG_6_DBM,
        NRF24L01_OUTPUT_POWER_0_DBM
};

struct nrf24l01_driver_api {
        /**
         * @brief установка адреса приема
         * @param dev экземпляр драйвера
         * @param addr массив из 5 байтов, представляющий адрес
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int8_t (*nrf24l01_set_rx_addr) (const struct device* dev,
                                     const uint8_t* addr);

        /**
         * @brief установка адреса передачи
         * @param dev экземпляр драйвера
         * @param addr массив из 5 байтов, представляющий адрес
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int8_t (*nrf24l01_set_tx_addr) (const struct device* dev,
                                     const uint8_t* addr);
        /**
         * @brief установка канала передачи
         * @param dev экземпляр драйвера
         * @param channel канал передачи. От 0 до 127
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int8_t (*nrf24l01_set_channel) (const struct device* dev,
                                     uint8_t channel);
        /**
         * @brief установка размера пакета данных
         * @param dev экземпляр драйвера
         * @param payload_len размер пакета данных. максимум - 32 байта
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int8_t (*nrf24l01_set_payload_len) (const struct device* dev,
                                         uint8_t payload_len);
        /**
         * @brief установка выходной мощности передатчика
         * @param dev экземпляр драйвера
         * @param output_power мощность передатчика
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int8_t (*nrf24l01_set_output_power) (const struct device* dev,
                                          enum nrf24l01_tx_power output_power);
        /**
         * @brief начать передачу данных
         * @param dev экземпляр драйвера
         * @param buf данные для передачи
         *
         * @retval 0: Успех
         * @retval -1: ошибка
         */
        int8_t (*nrf24l01_send) (const struct device* dev, const uint8_t* buf);

        /**
         * @brief проверить, продолжается ли передача
         * @param dev экземпляр драйвера
         *
         * @retval 0: передача данных завершена
         * @retval not_0: передача данных в процессе
         */
        int8_t (*nrf24l01_is_sending) (const struct device* dev);

        /**
         * @brief проверить, пришли ли новые данные
         * @param dev экземпляр драйвера
         *
         * @retval 0: данные не готовы
         * @retval not_0: данные готовы к чтению
         */
        int8_t (*nrf24l01_is_data_ready) (const struct device* dev);

        /**
         * @brief получить пришедшие данные
         * @param dev экземпляр драйвера
         *
         * @retval 0: Успех. Данные записаны в буфер
         * @retval -1: Ошибка
         */
        int8_t (*nrf24l01_get_data) (const struct device* dev, uint8_t* buf); 
};

/**
 * @brief установка адреса приема
 * @param dev экземпляр драйвера
 * @param addr массив из 5 байтов, представляющий адрес
 *
 * @retval 0: Успех
 * @retval -1: Ошибка
 */
static inline int8_t nrf24l01_set_rx_addr(const struct device* dev, const uint8_t* addr)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_set_rx_addr == NULL) {
                return -1;
        }

        return api->nrf24l01_set_rx_addr(dev, addr);
}

/**
 * @brief установка адреса передачи
 * @param dev экземпляр драйвера
 * @param addr массив из 5 байтов, представляющий адрес
 *
 * @retval 0: Успех
 * @retval -1: Ошибка
 */
static inline int8_t nrf24l01_set_tx_addr(const struct device* dev, const uint8_t* addr)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_set_tx_addr == NULL) {
                return -1;
        }

        return api->nrf24l01_set_tx_addr(dev, addr);
}

/**
 * @brief установка канала передачи
 * @param dev экземпляр драйвера
 * @param channel канал передачи. От 0 до 127
 *
 * @retval 0: Успех
 * @retval -1: Ошибка
 */
static inline int8_t nrf24l01_set_channel(const struct device* dev, uint8_t channel)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_set_channel == NULL) {
                return -1;
        }

        return api->nrf24l01_set_channel(dev, channel);
}

/**
 * @brief установка размера пакета данных
 * @param dev экземпляр драйвера
 * @param payload_len размер пакета данных. максимум - 32 байта
 *
 * @retval 0: Успех
 * @retval -1: Ошибка
 */
static inline int8_t nrf24l01_set_payload_len(const struct device* dev, uint8_t payload_len)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_set_payload_len == NULL) {
                return -1;
        }

        return api->nrf24l01_set_payload_len(dev, payload_len);
}

/**
 * @brief установка выходной мощности передатчика
 * @param dev экземпляр драйвера
 * @param output_power мощность передатчика
 *
 * @retval 0: Успех
 * @retval -1: Ошибка
 */
static inline int8_t nrf24l01_set_output_power(const struct device* dev,
                                               enum nrf24l01_tx_power output_power)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_set_output_power == NULL) {
                return -1;
        }

        return api->nrf24l01_set_output_power(dev, output_power);
}

/**
 * @brief начать передачу данных
 * @param dev экземпляр драйвера
 * @param buf данные для передачи
 *
 * @retval 0: Успех
 * @retval -1: ошибка
 */
static inline int8_t nrf24l01_send(const struct device* dev, const uint8_t* buf)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_send == NULL) {
                return -1;
        }

        return api->nrf24l01_send(dev, buf);
}

/**
 * @brief проверить, продолжается ли передача
 * @param dev экземпляр драйвера
 *
 * @retval 0: передача данных завершена
 * @retval 1: передача данных в процессе
 */
static inline int8_t nrf24l01_is_sending(const struct device* dev)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_is_sending == NULL) {
                return -1;
        }

        return api->nrf24l01_is_sending(dev);
}

static inline int8_t nrf24l01_is_data_ready(const struct device* dev)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_is_data_ready == NULL) {
                return -1;
        }

        return api->nrf24l01_is_data_ready(dev);
}

static inline int8_t nrf24l01_get_data(const struct device* dev, uint8_t* buf)
{
        struct nrf24l01_driver_api* api = (struct nrf24l01_driver_api* ) dev->api;

        if (api->nrf24l01_get_data == NULL) {
                return -1;
        }

        return api->nrf24l01_get_data(dev, buf);
}

#endif /* INCLUDE_DRIVERS_MODULES_NRF24L01_H */
