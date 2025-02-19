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

#ifndef INCLUDE_DRIVERS_INIT_UART_STM32_INIT_H
#define INCLUDE_DRIVERS_INIT_UART_STM32_INIT_H

#include <stm32f1xx.h>

#include <device.h>
#include <drivers/uart.h>

struct uart_stm32_settings;

/**
 * @brief Внутренняя конфигурация драйвера
 */
struct uart_stm32_config {
        /* uart_driver_config должен быть первым */
        struct uart_config common;

        /* Пользовательские настройки */
        const struct uart_stm32_settings* user_settings;

        /* Базовый адрес регистров */
        USART_TypeDef* uart_regs;
};

enum uart_stm32_state {
        INIT_STATE,
};

/**
 * @brief приватные данные драйвера
 */
struct uart_stm32_data {
        enum uart_stm32_state current_state;
};

/**
 * @name Публичный интерфейс драйвера uart_stm32
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
struct uart_stm32_settings {
        /**
         * Номер обслуживаемого драйверов UART-контроллера.
         * Нумерация соответствует обозначениям в reference manual.
         * USART1: 1
         * USART2: 2
         * ...
         */
        uint8_t uart_controller_num;
};

/**
 * @brief Конфигурирование драйвера
 *
 * Данная функция выполняет начальную низкоуровневую инициализацию драйвера.
 *
 * @param dev Структура, хранящая все внутрение данные
 */
int uart_stm32_init_driver(const struct device* dev);

/**
 * @brief Рабочий такт uart-драйвера
 *
 * Данная функция выполняет основную работу uart-драйвера (опрос регистров,
 * проверка буферов, ...,). Эта функция не блокирует поток выполнения.
 * Вызывать как можно чаще.
 *
 * @param dev uart-контроллер
 */
int uart_stm32_driver_process(const struct device* dev); 

/**
 * @}
 */

#endif /* INCLUDE_DRIVERS_INIT_UART_STM32_INIT_H */
