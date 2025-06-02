/**
 * @file 
 * @brief Реализация системного таймера
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "systick.h"

#include <stm32f1xx.h>

#include "config.h"


static volatile uint64_t afterboot_time_ms_var = 0;

void SysTick_Handler(void)
{
        afterboot_time_ms_var++;
        return;
}

void systick_init(void)
{
        /* Сколько отсчетов в таймер. -1 по reference manual */
        SysTick->LOAD = CONFIG_SYSTICK_CLOCK/1000 - 1;
        
        /* Включить прерывание по достижению нуля */
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

        /* Глобально включить прерывания */
        __enable_irq();

        /* Запустить таймер */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        return;
}

uint64_t afterboot_time_ms()
{
        return afterboot_time_ms_var;
}
