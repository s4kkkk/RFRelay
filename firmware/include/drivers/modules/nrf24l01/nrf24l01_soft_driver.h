/**
 * @file
 * @brief необходимое для низкоуровневой инициализации драйвера
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INCLUDE_DRIVERS_MODULES_NRF24L01_NRF24L01_SOFT_DRIVER_H
#define INCLUDE_DRIVERS_MODULES_NRF24L01_NRF24L01_SOFT_DRIVER_H

#include "nrf24l01.h"

#include <drivers/gpio/gpio.h>

struct nrf24l01_soft_driver_settings;

struct nrf24l01_soft_driver_config {
        /* Пользовательские настройки */
        const struct nrf24l01_soft_driver_settings* user_settings;
};

struct nrf24l01_soft_driver_data {
        uint8_t payload_len;
        uint8_t channel;
};

extern const struct nrf24l01_driver_api nrf24l01_soft_driver_api;

/**
 * @name Публичный интерфейс драйвера uart_stm32
 * @note Перед инициализацией драйвера необходимо настроить тактирование GPIO.
 * Драйвер не инициализирует тактирование!
 * @{
 */

/**
 * @brief структура, описывающая все используемые драйвером пины
 */
struct nrf24l01_soft_driver_pins {
        gpio_pin_t ce_pin;
        gpio_pin_t cs_pin;
        gpio_pin_t sck_pin;
        gpio_pin_t mosi_pin;
        gpio_pin_t miso_pin;
};

/**
 * @brief структура настроек драйвера
 * 
 * @note 
 */
struct nrf24l01_soft_driver_settings {
        /**
         * GPIO-контроллер, управляющий пинами, используемыми в качестве
         * SPI
         */
        const struct device* gpio_controller;

        /** Пины, используемые драйвером */
        struct nrf24l01_soft_driver_pins pins;
};

/**
 * @brief Создание экземпляра драйвера. Создание структуры device
 * @hideinitializer
 * @param dev_name имя создаваемого экземпляра устройства
 * @param settings указатель на настройки драйвера
 */
#define DEVICE_NRF24L01_SOFT_DRIVER_DEFINE(dev_name, settings)                  \
        static struct nrf24l01_soft_driver_config nrf24l01_cfg_## dev_name = {  \
                .user_settings = settings,                                      \
        };                                                                      \
                                                                                \
        static struct nrf24l01_soft_driver_data nrf24l01_data_## dev_name = {   \
                .payload_len = 1,                                               \
                .channel - 1,                                                   \
        };                                                                      \
                                                                                \
        static const struct device dev_name = {                                 \
                .name = #dev_name,                                              \
                .config = &(nrf24l01_cfg_## dev_name),                          \
                .api = &(nrf24l01_soft_driver_api),                             \
                .data = &(nrf24l01_data_## dev_name),                           \
        };                                                                      \

/**
 * @brief инициализация драйвера
 * @param dev устройство
 *
 * @retval 0: Успех
 * @retval -1: Ошибка
 */
int8_t nrf24l01_soft_driver_init(const struct device* dev);

/** @} */


#endif /* INCLUDE_DRIVERS_MODULES_NRF24L01_NRF24L01_SOFT_DRIVER_H */
