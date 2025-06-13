/**
 * @file
 * @brief 
 * Файл, описывающий формат пакетов, пересылаемых между устройствами
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef PACKETS_H
#define PACKETS_H

#include <stdint.h>

#pragma pack(push, 1)

/**
 * @brief Структура, описывающая формат пакета
 */
struct rp_packet {
        uint8_t packet_type;
        uint8_t flags;
        uint8_t check_sum;
};

#pragma pack(pop)

/**
 * @name Возможные значения полей пакета
 * @{
 */

/**
 * @name Типы пакетов
 * @{
 */

/**
 * @brief Пакет-маяк. Используется для перестройки частоты
 * @hideinitializer
 */
#define PACKET_TYPE_BEACON 0x00

/**
 * @brief Сервисный пакет. Используется для поддержания соединения
 * @hideinitializer
 */
#define PACKET_TYPE_SERVICE 0x01

/**
 * @brief Запрос состояния
 * @hideinitializer
 */
#define PACKET_TYPE_REQUEST_STATUS 0x02

/**
 * @brief Пакет с текущим состоянием. Такие пакеты отправляются только УУ
 * @hideinitializer
 */
#define PACKET_TYPE_STATUS 0x03

/** @} */

/**
 * @name Флаги
 * @{
 */

#define FLAG_NONE 0x00

/**
 * @brief 220В на линии
 * @hideinitializer
 */
#define FLAG_ON_LINE_POWER 0x01

/**
 * @brief 0В на линии
 * @hideinitializer
 */
#define FLAG_OFF_LINE 0x02

/**
 * @brief Ошибка приема (пакет поврежден). Необходимо повторить передачу
 * @hideinitializer
 */
#define FLAG_RETRY_REQ 0x03

/** @} */

/** @} */

/**
 * @brief Проверка контрольной суммы
 *
 * @param packet Указатель на структуру пакета
 *
 * @retval 0: Контрольная сумма некорректна. Пакет поврежден
 * @retval 1: Контрольная сумма корректна.
 * @retval -1: Внутренняя ошибка
 */
int8_t packet_has_correct_checksum(const struct rp_packet* packet);

/**
 * @brief Вычисление и вставка контрольной суммы пакета
 *
 * Вычисляет контрольную сумму от пакета и вставляет эту контрольную сумму
 * в поле пакета. После вызова этой функции в пакете будет находиться корректная
 * контрольная сумма
 *
 * @param packet Указатель на структуру пакета
 *
 * @retval 0: Успешное выполнение
 * @retval -1: Внутренняя ошибка
 */
int8_t packet_insert_checksum(struct rp_packet* packet);

#endif // PACKETS_H
