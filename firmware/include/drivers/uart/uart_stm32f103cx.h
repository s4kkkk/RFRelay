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
        /* uart_config должен быть первым */
        struct uart_config common;

        /* Пользовательские настройки */
        const struct uart_stm32_settings* user_settings;

        /* Базовый адрес регистров */
        USART_TypeDef* uart_regs;
};

enum uart_stm32_reciever_state {
        RX_DISABLED,
        RX_WAIT_FOR_READ,
        RX_RECIEVING,
};

enum uart_stm32_transiever_state {
        TX_IDLE,
        TX_TRANSMITTING,
        TX_WAIT_TO_TXE,
        TX_WAIT_TO_TC
};

#define BUFFER_LEN 32

/**
 * @brief приватные данные драйвера
 */

#define RX_READED (0b1 << 0)
#define RX_DISABLE (0b1 << 1)
#define RX_ENABLE (0b1 << 2)

#define TX_START_TRANSMISSION (0b1 << 0)
#define TX_ABORTED (0b1 << 1)

struct uart_stm32_data {
        /* RX */
        enum uart_stm32_reciever_state rx_state;
        /* Флаги состояний */
        uint8_t rx_state_flags;
        /* Буфер приема */
        uint8_t* rx_buffer;
        /* Размер буфеа приема */
        size_t rx_buffer_size;
        /* Текущий передаваемый байт */
        size_t rx_buffer_cur_byte;

        /* TX */
        enum uart_stm32_transiever_state tx_state;
        /* Флаги состояний */
        uint8_t tx_state_flags;
        /* Передаваемый буфер */
        const uint8_t* tx_data;
        /* Размер передаваемого буфера */
        size_t tx_data_len;
        /* Текущий передаваемый байт из буфера */
        size_t tx_data_cur_byte;
};

#undef BUFFER_LEN 

extern const struct uart_driver_api uart_stm32_driver_api;

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

        /**
         * Установленная частота тактирования для используемого
         * контроллера в Гц. Настройку тактирования должен
         * выполнять пользовательский код
         */
        uint32_t uart_controller_clk;
};

/**
 * @brief Создание экземпляра драйвера. Создание структуры device
 * @hideinitializer
 * @param dev_name имя создаваемого экземпляра устройства
 * @param settings указатель на структуру типа uart_stm32_settings
 */
#define DEVICE_UART_STM32_DEFINE(dev_name, settings)                    \
        static struct uart_stm32_config uart_stm32_cfg_## dev_name = {  \
                .user_settings = settings,                              \
        };                                                              \
                                                                        \
        static struct uart_stm32_data uart_stm32_data_## dev_name ;     \
                                                                        \
        static const struct device dev_name = {                         \
                .name = #dev_name,                                      \
                .config = &(uart_stm32_cfg_## dev_name),                \
                .api = &(uart_stm32_driver_api),                        \
                .data = &(uart_stm32_data_## dev_name),                 \
        };                                                              \
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
