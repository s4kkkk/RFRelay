/**
 * @file 
 * @brief Описание подсистемы UART
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INCLUDE_DRIVERS_UART_H
#define INCLUDE_DRIVERS_UART_H

#include <stdint.h>
#include <stddef.h>

#include <device.h>

/**
 * @brief API подсистемы UART
 * @defgroup uart_api API подсистемы UART
 * @{
 */

/**
 * @name Флаги настройки ввода-вывода
 * @{
 */

/** @} */

/** @brief Размер кадра данных */
enum uart_config_character_bits {
        UART_CONFIG_CHARACTER_BITS_8,
        UART_CONFIG_CHARACTER_BITS_9
};

/** @brief Количество стоп-битов */
enum uart_config_stop_bits {
        UART_CONFIG_STOP_BITS_0_5,
        UART_CONFIG_STOP_BITS_1,
        UART_CONFIG_STOP_BITS_1_5,
        UART_CONFIG_STOP_BITS_2
};

/**
 * @brief Структура конфигурации UART-контроллера
 *
 * Общая для всех драйверов структура. Первый элемент объекта, указываемый полем
 * config в struct device (см. @ref device_api), должен являться этой структурой.
 * То есть, поле config в struct device должно указывать на эту структуру
 */
struct uart_config {
        /** @brief Бодрейт */
        uint32_t baudrate;

        /** @brief Размер передаваемых/принимаемых кадров */
        enum uart_config_character_bits character_bits;

        /** @brief Кол-во стоп-битов */
        enum uart_config_stop_bits stop_bits;
};

/**
 * @brief Блок, описывающий необходимые для реализации драйвером компоненты
 * @defgroup uart_driver_api Компоненты, необходимые для реализации драйвером
 * @ingroup uart_api
 * @{
 */

struct uart_driver_api {
        /**
         * @brief Выполняет конфигурирование uart-контроллера
         * @param dev uart-контроллер
         * @param config заполненная структура конфигурации
         *
         * @retval 0: Успех
         * @retval 1: Внутренняя ошибка
         */
        int (*uart_configure) (const struct device* dev,
                               const struct uart_config* config);

        /**
         * @brief Установить буфер приема
         * @param dev uart-контроллер
         * @param buffer - указатель на буфер приема
         * @param len длина буфера
         *
         * @retval 0: Успех
         * @reval -1: Внутренняя ошибка
         */
        int (*uart_set_rx_buffer) (const struct device* dev,
                                   uint8_t* buffer,
                                   size_t len);

        /**
         * @brief Включить приемник
         *
         * @retval 0: Успешно
         * @retval -1: Внутренняя ошибка
         */
        int (*uart_rx_enable) (const struct device* dev);

        /**
         * @brief Выключить приемник
         *
         * @retval 0: Успешно
         * @retval -1: Внутренняя ошибка
         */
        int (*uart_rx_disable) (const struct device* dev);

        /**
         * @brief Проверяет, доступны ли данные для считывания
         * @param dev uart-контроллер
         *
         * @retval 0: Данные не доступны
         * @retval не 0: данные доступны
         */
        uint8_t (*uart_is_data_available) (const struct device* dev);

        /**
         * @brief Попытка чтения @p len байтов
         *
         * @return Кол-во считанных байт
         */
        size_t (*uart_rx) (const struct device* dev, void* buffer, size_t len);

        /**
         * @brief Включить передатчик
         *
         * @retval 0: Успешно
         * @retval -1: Внутренняя ошибка
         */
        uint8_t (*uart_tx_enable) (const struct device* dev);

        /**
         * @brief Выключить передатчик
         *
         * @retval 0: Успешно
         * @retval -1: Внутренняя ошибка
         */
        uint8_t (*uart_tx_disable) (const struct device* dev);

        /**
         * @brief Проверяет, готов ли контроллер к передаче
         * 
         * @retval 0: Передатчик занят
         * @retval 1: Передатчик готов
         */
        uint8_t (*uart_is_transmitter_ready) (const struct device* dev);

        /**
         * @brief Запуск передачи @p len байтов
         * 
         * @retval 0: Передача запущена
         * @retval 1: Передатчик занят
         * @retval -1: Внутренняя ошибка
         */
        int (*uart_tx) (const struct device* dev, const void* buffer, size_t len);

        /**
         * @brief ожидать завершение передачи 
         *
         * @retval 0: Передача завершена
         * @retval 1: Во время передачи произошла ошибка
         */
        int (*uart_wait_for_tx_complete) (const struct device* dev);

        /**
         * @brief Прервать текущую передачу
         *
         * @retval 0: Успешно
         * @retval -1: Внутренняя ошибка
         */
        int (*uart_tx_abort) (const struct device* dev);

};

/** @} */

/**
 * @brief Установить буфер приема
 * @param dev uart-контроллер
 * @param buffer - указатель на буфер приема
 * @param len длина буфера
 *
 * @retval 0: Успех
 * @reval -1: Внутренняя ошибка
 */
static inline int uart_set_rx_buffer(const struct device* dev,
                                     uint8_t* buffer,
                                     size_t len)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_set_rx_buffer == NULL) {
                return -1;
        }
        return api->uart_set_rx_buffer(dev, buffer, len);
}

/**
 * @brief Выполняет конфигурирование uart-контроллера
 * @param dev uart-контроллер
 * @param config - заполненная структура конфигурации
 *
 * @retval 0: Успех
 * @retval 1: Внутренняя ошибка
 */
static inline int uart_configure(const struct device* dev,
                                 const struct uart_config* config)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_configure(dev, config);
}

/**
 * @brief Включить приемник
 *
 * @retval 0: Успешно
 * @retval -1: Внутренняя ошибка
 */
static inline int uart_rx_enable(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_rx_enable(dev);
}

/**
 * @brief Выключить приемник
 *
 * @retval 0: Успешно
 * @retval -1: Внутренняя ошибка
 */
static inline int uart_rx_disable(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_rx_disable(dev);
}

/**
 * @brief Проверяет, доступны ли данные для считывания
 * @param dev uart-контроллер
 *
 * @retval 0: Данные не доступны
 * @retval не 0: данные доступны
 */
static inline uint8_t uart_is_data_available(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_is_data_available(dev);
}

/**
 * @brief Попытка чтения @p len байтов
 *
 * @return Кол-во считанных байт
 */
static inline size_t uart_rx(const struct device* dev, void* buffer, size_t len)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_rx(dev, buffer, len);
}

/**
 * @brief Включить передатчик
 *
 * @retval 0: Успешно
 * @retval -1: Внутренняя ошибка
 */
static inline uint8_t uart_tx_enable(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_tx_enable == NULL) {
                return -1;
        }
        return api->uart_tx_enable(dev);
}

/**
 * @brief Выключить передатчик
 *
 * @retval 0: Успешно
 * @retval -1: Внутренняя ошибка
 */
static inline uint8_t uart_tx_disable(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_tx_disable == NULL) {
                return -1;
        }
        return api->uart_tx_disable(dev);
}

/**
 * @brief Проверяет, готов ли контроллер к передаче
 * 
 * @retval 0: Передатчик занят
 * @retval 1: Передатчик готов
 */
static inline uint8_t uart_is_transmitter_ready(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_is_transmitter_ready(dev);
}

/**
 * @brief Запуск передачи @p len байтов
 * 
 * @retval 0: Передача запущена
 * @retval 1: Передатчик занят
 * @retval -1: Внутренняя ошибка
 */
static inline int uart_tx(const struct device* dev, const void* buffer, size_t len)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_tx(dev, buffer, len);
}

static inline int uart_wait_for_tx_complete(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_wait_for_tx_complete == NULL) {
                return -1;
        }
        return api->uart_wait_for_tx_complete(dev);

}

/**
 * @brief Прервать текущую передачу
 *
 * @retval 0: Успешно
 * @retval -1: Внутренняя ошибка
 */
static inline int uart_tx_abort(const struct device* dev)
{
        struct uart_driver_api* api = (struct uart_driver_api* ) dev->api;

        if (api->uart_configure == NULL) {
                return -1;
        }
        return api->uart_tx_abort(dev);
}

/**
 * @}
 */

#endif /* INCLUDE_DRIVERS_UART_H */
