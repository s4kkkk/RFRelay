/**
 * @file 
 * @brief Описание подсистемы GPIO
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INCLUDE_DRIVERS_GPIO_H
#define INCLUDE_DRIVERS_GPIO_H

#include <stddef.h>

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

/** @brief Тип для хранения значения пинов.
 *
 * Пин с номером n находится в состоянии логической "1" тогда, когда установлен бит (1U << n).
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
        int (*pins_configure)(const struct device* port,
                              gpio_pins_t pins,
                              gpio_flags_t flags);

        /**
         * @brief Получить текущую конфигурацию пина
         *
         * @param port Указатель на структуру device используемого драйвера (device)
         * @param pin Номер пина
         * @param flags Указатель на флаговую переменную
         *
         * @retval 0 Успешное выполнение
         * @retval -1 Ошибка
         */
        int (*pin_get_config)(const struct device* port,
                              gpio_pin_t pin,
                              gpio_flags_t* flags);

        /**
         * @brief Получить состояние всех пинов
         * @param port драйвер, управляющий портом
         * @param values указатель на контейнер, хранящий значения пинов
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_read_raw)(const struct device* port, gpio_pins_values_t* values);

        /**
         * @brief Установить значения пинов @p pins
         * @param port драйвер, управляющий портом
         * @param pins набор пинов, для которых выполняется действие
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_set_raw)(const struct device* port, gpio_pins_t pins);

        /**
         * @brief Сбросить значения пинов @p pins
         * @param port драйвер, управляющий портом
         * @param pins набор пинов, для которых выполняется действие
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_clear_raw)(const struct device* port, gpio_pins_t pins);

        /**
         * @brief Переключить значения пинов @p pins на противоположное
         * @param port драйвер, управляющий портом
         * @param pins набор пинов, для которых выполняется действие
         *
         * @retval 0: Успех
         * @retval -1: Ошибка
         */
        int (*pins_toggle)(const struct device* port, gpio_pins_t pins);
};

/** @} */

/**
 * @brief Конфигурирование одного пина
 *
 * @param port Указатель на структуру device используемого драйвера (device)
 * @param pin Номер пина
 * @param flags Флаги. См. Флаги настройки GPIO пинов
 *
 * @retval 0 Успешное выполнение
 * @retval -1 Ошибка
 */
int gpio_pin_configure(const struct device* port,
                                     gpio_pin_t pin,
                                     gpio_flags_t flags);

/**
 * @brief Получить текущую конфигурацию пина
 *
 * @param port Указатель на структуру device используемого драйвера (device)
 * @param pin Номер пина
 * @param flags Указатель на флаговую переменную
 *
 * @retval 0 Успешное выполнение
 * @retval -1 Ошибка
 */
int gpio_pin_get_config(const struct device* port,
                                      gpio_pin_t pin,
                                      gpio_flags_t* flags);

/**
 * @brief Установить логический уровень пина
 *
 * @param port Указатель на структуру device используемого драйвера (device)
 * @param pin Номер пина
 * @param value Устанавливаемое значение. Если @p value = 0, то устанавливается
 * логический "0", в противном случае устанавливается логическая "1"
 *
 * @retval 0 Успешное выполнение
 * @retval -1 Ошибка
 */
int gpio_pin_set(const struct device* port,
                               gpio_pin_t pin,
                               uint8_t value);

/**
 * @brief Получить текущий логический уровень пина
 *
 * @param port Указатель на структуру device используемого драйвера (device)
 * @param pin Номер пина
 *
 * @retval 0 состояние пина - логический "0"
 * @retval 1 состояние пина - логическая "1"
 * @retval -1 Ошибка
 */
int gpio_pin_read(const struct device* port, gpio_pin_t pin);

/**
 * @brief Переключить логический уровень пина на противоположный
 *
 * @param port Указатель на структуру device используемого драйвера (device)
 * @param pin Номер пина, который конфигурируется
 *
 * @retval 0 Успешное выполнение
 * @retval -1 Ошибка
 */
int gpio_pin_toggle(const struct device* port, gpio_pin_t pin);

/**
 * @}
 */

#endif /* INCLUDE_DRIVERS_GPIO_H */
