/**
 * @file 
 * @brief Реализация таймеров
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "timer.h"

#include <stddef.h>

#include "systick.h"

int8_t is_timer_elapsed(const struct timer_t *timer)
{
        if (timer == NULL) {while(1);}

        if ( (afterboot_time_ms() - timer->prev_time_ms) >=
                timer->timer_delay_ms) {
                return 1;
        }
        else {
                return 0;
        }
}

int8_t setup_timer(struct timer_t *timer, const uint64_t delay)
{
        if (timer == NULL) {return -1;}

        timer->timer_delay_ms = delay;

        return 0;
}

uint64_t get_timer_delay(const struct timer_t *timer)
{
        if (timer == NULL) {while(1);}

        return timer->timer_delay_ms;
}

int8_t reset_timer(struct timer_t *timer)
{
        if (timer == NULL) {return -1;}

        timer->prev_time_ms = afterboot_time_ms();
        return 0;
}
