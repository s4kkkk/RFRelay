/**
 * @file 
 * @brief Необходимое для низкоуровневой инициализации драйвера
 *
 * Описанные тут функции и структуры данных используются на этапе
 * низкоуровневой инициализации драйверов (создании и регистрации экзепляров
 * struct device)
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INCLUDE_DRIVERS_INIT_GPIO_STM32_INIT_H
#define INCLUDE_DRIVERS_INIT_GPIO_STM32_INIT_H

#include <stm32f1xx.h>

#include <device.h>
#include <drivers/gpio.h>


struct gpio_stm32_settings;

/**
 * @brief Внутренняя конфигурация драйвера
 */
struct gpio_stm32_config {
        /* gpio_driver_config должен быть первым */
        struct gpio_driver_config common;

        /* Пользовательские настройки */
        const struct gpio_stm32_settings* user_settings;

        /* Базовый адрес регистров */
        GPIO_TypeDef* gpio_regs;
};

/**
 * @brief Данные драйвера
 */
struct gpio_stm32_data {
        uint32_t flag;
};

extern const struct gpio_driver_api gpio_stm32_driver_api;

/**
 * @name Инициализация драйвера gpio_stm32
 * @note Перед инициализацией драйвера необходимо настроить тактирование GPIO.
 * Драйвер не инициализирует тактирование!
 * @{
 */

/**
 * @brief Структура, которую нужно заполнить перед инициализацией
 * драйвера
 *
 * @note Данные из этой структуры постоянны, поэтому она может быть
 * создана в пользовательском коде с квалификатором const
 */
struct gpio_stm32_settings {
        /**
         * Номер обслуживаемого порта. Нумерация соответствует обозначениям
         * в reference manual.
         * GPIOA: 0
         * GPIOB: 1
         * ...
         */
        uint8_t port;
};

/**
 * @brief Создание экземпляра драйвера. Создание структуры device
 * @hideinitializer
 * @param dev_name имя создаваемого экземпляра драйвера. По этому имени можно
 * найти этот экземпляр в реестре
 */
#define DEVICE_GPIO_STM32_DEFINE(dev_name, settings)                        \
        static struct gpio_stm32_config gpio_stm32_cfg_## dev_name = {      \
                .user_settings = settings,                              \
        };                                                              \
                                                                        \
                                                                        \
        static const struct device dev_name = {        \
                .name = #dev_name,                                          \
                .config = &(gpio_stm32_cfg_## dev_name),                    \
                .api = &gpio_stm32_driver_api,                          \
        }                                                               \


/**
 * @brief Конфигурирование драйвера
 *
 * Данная функция выполняет начальную низкоуровневую инициализацию драйвера.
 *
 * @param dev Структура, хранящая все внутрение данные
 */
int gpio_stm32_init_driver(const struct device* dev);

/**
 * @}
 */

#endif /* INCLUDE_DRIVERS_INIT_GPIO_STM32_INIT_H */
