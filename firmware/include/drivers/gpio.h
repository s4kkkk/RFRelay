/**
 * @file 
 * @brief Описание подсистемы device
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <device.h>

/**
 * @brief API подсистемы GPIO
 * @defgroup gpio_api API подсистемы GPIO
 * @{
 */

/**
 * @name Флаги настройки GPIO пинов
 * @{
 */

/** Отключить пин */
#define GPIO_DISCONNECTED (1U << 31)

/** Режим "ВХОД" */
#define GPIO_INPUT (1U << 0)

/** Режим "ВХОД" с подтяжкой питания */
#define GPIO_INPUT_PULLUP (1U << 1)

/** Режим "ВЫХОД" */
#define GPIO_OUTPUT (1U << 2)

/** @} */

/**
 * @brief Тип для хранения флагов GPIO. См. Флаги настройки GPIO пинов
 */
typedef uint32_t gpio_flags_t;

/**
 * @brief Тип для хранения набора пинов
 *
 * Пин с номером n присутствует в наборе, если установлен бит (1U << n)
 */
typedef uint32_t gpio_pins_t;

/**
 * @brief Тип для хранения индекса пина
 */
typedef uint8_t gpio_pin_t;

/** @brief Тип для хранения значения пинов. Пин с номером n находится в 
 * состоянии логической "1" тогда, когда установлен бит (1U << n).
 * Пин с номером n находится в состоянии логического "0" тогда, когда
 * сброшен бит (1U << n)
 */
typedef uint32_t gpio_pins_values_t;

/**
 * @brief
 * Общая для всех драйверов структура. Первый элемент объекта, указываемый полем
 * config в struct device (см. @ref device_api), должен являться этой структурой.
 * То есть, поле config в struct device должно указывать на эту структуру
 */
struct gpio_driver_config {
        /** Указывает набор поддерживаемых текущим экземпляром драйвера (device)
         * пинов
         */
        gpio_pins_t port_pins;
};

struct gpio_driver_api;

//TODO добавить функции GPIO-API

/**
 * @brief Блок, описывающий необходимые для реализации драйвером компоненты
 * @defgroup gpio_driver_api Компоненты, необходимые для реализации драйвером
 * @ingroup gpio_api
 * @{
 */

/**
 * @brief Структура, описывающая необходимые функции, которые должны быть
 * реализованы драйвером
 */
struct gpio_driver_api {
        /**
         * @brief Выполняет конфигурирование пинов @p pins
         * @param port драйвер, управляющий портом
         * @param pins набор пинов, для которых применяются флаги @p flags
         * @param flags флаги пинов (см. Флаги настройки GPIO пинов)
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_configure)(const struct device* port, gpio_pins_t pins,
                        gpio_flags_t flags);

        /**
         * @brief Получить состояние всех пинов
         * @param port драйвер, управляющий портом
         * @param values указатель на контейнер, хранящий значения пинов
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_get_raw)(const struct device* port,
                        gpio_pins_values_t* values);

        /**
         * @brief Установить значения пинов @p pins
         * @param port драйвер, управляющий портом
         * @param pins набор пинов, для которых выполняется действие
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_set_raw)(const struct device* port,
                        gpio_pins_t pins);

        /**
         * @brief Сбросить значения пинов @p pins
         * @param port драйвер, управляющий портом
         * @param pins набор пинов, для которых выполняется действие
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_clear_raw)(const struct device* port,
                        gpio_pins_t pins);

        /**
         * @brief Переключить значения пинов @p pins на противоположное
         * @param port драйвер, управляющий портом
         * @param pins набор пинов, для которых выполняется действие
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_toggle)(const struct device* port,
                        gpio_pins_t pins);
};

/** @} */

/**
 * @}
 */
