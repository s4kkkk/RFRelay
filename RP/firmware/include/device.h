/**
 * @file 
 * @brief Описание подсистемы device
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INCLUDE_DEVICE_H
#define INCLUDE_DEVICE_H

#include <stdint.h>

/**
 * @brief API подсистемы device
 * @defgroup device_api API подсистемы device
 * @{
 */

/**
 * @brief Структура device, используемая для описания экземпляров драйверов
 */
struct device {
        /** Имя устройства, и, как следствие, экземпляра драйвера */
        const char* name;

        /** Адрес структуры данных конфигурации экземпляра драйвера */
	void* config;

	/** Адрес структуры, хранящей указатели на функции API-подсистемы,
         *  реализуемые в драйвере
         */
	const void* api;

	/** Адрес приватных данных экземпляра драйвера */
	void* data;
};

/**
 * @brief Функция для регистрации устройства (экземпляра драйвера) в реестре
 * устройств
 *
 * @param dev Указатель на структуру устройства, которое будет зарегистрировано
 *
 * @retval 0: Успешная регистрация
 * @retval -1: Ошибка: внутренний реестр устройств переполнен
 */
int8_t device_register(const struct device* dev);

/**
 * @brief Функция для получения указателя на struct device по имени устройства
 *
 * @param name Имя устройства, присутствующего в реестре
 *
 * @return Указатель на struct device или NULL, если устройство не найдено
 */
const struct device* device_get_by_name(const char* name);

/**
 * @}
 */
#endif /* INCLUDE_DEVICE_H */
