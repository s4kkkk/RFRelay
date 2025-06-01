/**
 * @file 
 * @brief Таймеры для реализаций временных задержек
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/**
 * @brief структура, хранящая внутренние поля таймера
 */
struct timer_t {
        uint64_t prev_time_ms;
        uint64_t timer_delay_ms;
};

/**
 * @brief проверить, истек ли таймер
 *
 * @retval 0: таймер не истек
 * @retval не 0: таймер истек
 */
int8_t is_timer_elapsed(const struct timer_t* timer);

/**
 * @brief установить задержку для таймера
 *
 * @retval 0: задержка установлена успешно
 * @retval -1: внутренняя ошибка
 */
int8_t setup_timer(struct timer_t* timer, const uint64_t delay);

/**
 * @brief получить текущую установленную для таймера задержку
 *
 * @return значение текущей задержки
 */
uint64_t get_timer_delay(const struct timer_t* timer);

/**
 * @brief сбросить таймер
 *
 * После сброса таймер сразу начинает работу, поскольку он основан на внутреннем таймере SysTick,
 * который работает постоянно.
 *
 * @retval 0: таймер успешно сброшен
 * @retval -1: внутренняя ошибка
 */
int8_t reset_timer(struct timer_t* timer);

#endif /* TIMER_H */


