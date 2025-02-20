/**
 * @file 
 * @brief Реализация UART-драйвера для stm32f103c8
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <math.h>

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

/**
 * @brief установка размера пакета
 */
static inline void set_packet_size(struct uart_stm32_config* inner_config)
{
        switch (inner_config->common.character_bits) {
                case UART_CONFIG_CHARACTER_BITS_8:
                        inner_config->uart_regs->CR1 &=
                                ~(USART_CR1_M);
                        break;
                case UART_CONFIG_CHARACTER_BITS_9:
                        inner_config->uart_regs->CR1 |=
                                USART_CR1_M;
                        break;
                default:
                        break;
        }
        return;
}

/**
 * @brief установка кол-ва стоп-битов
 */
static inline void set_stop_bits(struct uart_stm32_config* inner_config)
{
        inner_config->uart_regs->CR2 &= ~(USART_CR2_STOP_Msk);
        switch (inner_config->common.stop_bits) {
                case UART_CONFIG_STOP_BITS_0_5:
                        inner_config->uart_regs->CR2 |= 
                                (0b01<<USART_CR2_STOP_Pos);
                        break;

                case UART_CONFIG_STOP_BITS_1_5:
                        inner_config->uart_regs->CR2 |=
                                (0b11<<USART_CR2_STOP_Pos);
                        break;
                case UART_CONFIG_STOP_BITS_2:
                        inner_config->uart_regs->CR2 |=
                                (0b10<<USART_CR2_STOP_Pos);
                        break;
                default:
                        break;
        }
        return;
}

/**
 * @brief установка бодрейта
 */
static inline int set_baudrate(struct uart_stm32_config* inner_config)
{
        uint32_t usart_clk = inner_config->user_settings->uart_controller_clk;
        if (usart_clk == 0)
                return -1;

        float usart_div = ((float ) usart_clk ) /
                (16*inner_config->common.baudrate);

        uint16_t div_fraction = 0;
        uint16_t div_mantissa = (int) usart_div;

        div_fraction = rintf( 16*(usart_div - ((int) usart_div)) );
        if (div_fraction == 16) {
                div_fraction = 0;
                div_mantissa++;
        }

        uint16_t usart_brr = (div_mantissa << 4) | div_fraction;

        inner_config->uart_regs->BRR = usart_brr;

        return 0;
}

static int stm32_uart_configure(const struct device* dev,
                          const struct uart_config* config)
{
        struct uart_stm32_config* inner_config = 
                (struct uart_stm32_config* ) dev->config;
        inner_config->common = *config;

        /* Включение UART */
        inner_config->uart_regs->CR1 |= USART_CR1_UE;

        set_packet_size(inner_config);

        set_stop_bits(inner_config);

        int ret;

        ret = set_baudrate(inner_config);
        if (ret != 0)
                return ret;

        return 0;
}

static int stm32_uart_rx_enable(const struct device* dev)
{
        struct uart_stm32_config* inner_config = 
                (struct uart_stm32_config* ) dev->config;

        inner_config->uart_regs->CR1 |= USART_CR1_RE;
        return 0;
}

static int stm32_uart_rx_disable(const struct device* dev)
{
        struct uart_stm32_config* inner_config = 
                (struct uart_stm32_config* ) dev->config;

        inner_config->uart_regs->CR1 &= ~(USART_CR1_RE);
        return 0;
}

static uint8_t stm32_uart_is_data_available(const struct device* dev)
{
        return 0;
}

static size_t stm32_uart_rx(const struct device* dev, void* buffer, size_t len)
{
        return 0;
}

static uint8_t stm32_uart_is_transmitter_ready(const struct device* dev)
{
        return 0;
}

static int stm32_uart_tx(const struct device* dev, const void* buffer, size_t len)
{
        struct uart_stm32_config* config = 
                (struct uart_stm32_config* ) dev->config;
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;
        
        if ( (buffer == NULL) && (len == 0) )
                return -1;
        
        data->current_tx_data = buffer;
        data->current_num_tx_bytes = len;
        data->current_tx_byte = 0;
        data->current_tx_state = TX_TRANSMITTING;

        if (config->common.io_flags & UART_O_BLOCK) {
                while (!uart_stm32_driver_process(dev));
        }

        return 0;
}

static int stm32_uart_tx_abort(const struct device* dev)
{
        return 0;
}

static inline int process_reciever(const struct device* dev)
{
        return 0;
}

static inline int process_transiever(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;
        struct uart_stm32_config* config = 
                (struct uart_stm32_config* ) dev->config;

        switch (data->current_tx_state) {
                case TX_IDLE:
                        return 1;

                case TX_TRANSMITTING: {
                        if (data->current_tx_byte >= data->current_num_tx_bytes) {
                                data->current_tx_state = TX_WAIT_TO_TC;
                                return 0;
                        }
                        uint8_t byte_to_tx = 
                                data->current_tx_data[data->current_tx_byte];
                        data->current_tx_byte++;

                        config->uart_regs->CR1 |= USART_CR1_TE;
                        config->uart_regs->DR = byte_to_tx;

                        data->current_tx_state = TX_WAIT_TO_TXE;
                        return 0;
                }

                case TX_WAIT_TO_TXE: {
                        if (config->uart_regs->SR & USART_SR_TXE_Msk)
                                data->current_tx_state = TX_TRANSMITTING;
                        return 0;
                }

                case TX_WAIT_TO_TC : {
                        if (config->uart_regs->SR & USART_SR_TC_Msk) {
                                /* Передача завершена */
                                data->current_tx_state = TX_IDLE;
                                config->uart_regs->CR1 &= ~(USART_CR1_TE);
                        }
                        return 0;
                }       
        }
}

int uart_stm32_driver_process(const struct device* dev)
{
        return process_reciever(dev) && process_transiever(dev);
}
