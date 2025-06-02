/**
 * @file 
 * @brief Системный таймер
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/**
 * @brief инициализация системного таймера
 */
void systick_init(void);

/**
 * @brief получить прошедшее времяп в мс после старта МК
 *
 * @return время в мс после старта МК
 */
uint64_t afterboot_time_ms(void);

#endif /* SYSTICK_H */
