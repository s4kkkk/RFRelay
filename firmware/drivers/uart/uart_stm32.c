/**
 * @file 
 * @brief Реализация UART-драйвера для stm32f103c8
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "stm32f103xb.h"
#include <drivers/uart.h>
#include <stm32f1xx.h>

#include <drivers_init/uart_stm32_init.h>


/**
 * @brief Настройка состояния GPIO-выводов
 */
static inline void init_pins_state(const struct device* dev)
{
        struct uart_stm32_config* config = (struct uart_stm32_config* ) dev->config;

        if (config->user_settings->uart_controller_num == 1) {
                /*
                 * PA9: TX, AF push-pull
                 * PA10: RX, input-floating
                 */

                /* сбросить все биты */
                GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9 |
                                GPIO_CRH_CNF10 |GPIO_CRH_MODE10);

                /* PA9 -> AF push-pull */
                GPIOA->CRH |= (0b11 << GPIO_CRH_MODE9_Pos);
                GPIOA->CRH |= (0b10 << GPIO_CRH_CNF9_Pos);

                /* PA10 -> input-floating */
                GPIOA->CRH |= (0b01 << GPIO_CRH_CNF10_Pos);
        }
        else if (config->user_settings->uart_controller_num == 2) {
                /*
                 * PA2: TX, AF push-pull
                 * PA3: RX, input-floating
                 */

                /* сбросить все биты */
                GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2 |
                                GPIO_CRL_CNF3 |GPIO_CRL_MODE3);

                /* PA2 -> AF push-pull */
                GPIOA->CRL |= (0b11 << GPIO_CRL_MODE2_Pos);
                GPIOA->CRL |= (0b10 << GPIO_CRL_CNF2_Pos);

                /* PA10 -> input-floating */
                GPIOA->CRL |= (0b01 << GPIO_CRL_CNF3_Pos);
        }
        else if (config->user_settings->uart_controller_num == 3) {
                /*
                 * PB10: TX, AF push-pull
                 * PB11: RX, input-floating
                 */

                /* сбросить все биты */
                GPIOB->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10 |
                                GPIO_CRH_CNF11 |GPIO_CRH_MODE11);

                /* PB10 -> AF push-pull */
                GPIOB->CRH |= (0b11 << GPIO_CRH_MODE10_Pos);
                GPIOB->CRH |= (0b10 << GPIO_CRH_CNF10_Pos);

                /* PB11 -> input-floating */
                GPIOB->CRH |= (0b01 << GPIO_CRH_CNF11_Pos);

        }
        return;
}

/**
 * @brief Инициалиазация драйвера
 */
int uart_stm32_init_driver(const struct device *dev)
{
        struct uart_stm32_config* config = (struct uart_stm32_config* ) dev->config;

        switch (config->user_settings->uart_controller_num) {

                case 1:
                        config->uart_regs = USART1;
                        break;
                case 2:
                        config->uart_regs = USART2;
                        break;
                case 3:
                        config->uart_regs = USART3;
                        break;
                default:
                        return -1;
                        break;
        }

        init_pins_state(dev);

        return 0;
}
