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

#include <drivers/uart/uart_stm32f103cx.h>

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
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

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

        data->rx_state = RX_DISABLED;
        data->rx_state_flags = 0;
        data->tx_state = TX_IDLE;
        data->tx_state_flags = 0;

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

static int stm32_uart_set_rx_buffer(const struct device* dev,
                                    uint8_t* buffer,
                                    size_t len)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

        if ( (buffer == NULL) || len == 0)
                return -1;

        data->rx_buffer = buffer;
        data->rx_buffer_size = len;
        data->rx_buffer_cur_byte = 0;

        return 0;
}

static int stm32_uart_rx_enable(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

        data->rx_state_flags |= RX_ENABLE;
        return 0;
}

static int stm32_uart_rx_disable(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

        data->rx_state_flags |= RX_DISABLE;
        return 0;
}

static int process_reciever(const struct device* dev);

static uint8_t stm32_uart_is_data_available(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

        process_reciever(dev);

        if (data->rx_buffer_cur_byte != 0) {
                return 1;
        }
        return 0;
}

static size_t stm32_uart_rx(const struct device* dev, void* buffer, size_t len)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

        size_t readed = 0;

        if (data->rx_buffer_cur_byte != 0) {
                /* считанные байты есть */
                for (size_t i = 0; i < data->rx_buffer_cur_byte; i++) {
                        if (i >= len)
                                break;
                        ((uint8_t *) buffer)[i] = data->rx_buffer[i];
                        readed ++;
                }

                data->rx_buffer_cur_byte = 0;
                data->rx_state_flags |= RX_READED;
        }
        return readed;
}

static uint8_t stm32_uart_tx_enable(const struct device* dev)
{
        struct uart_stm32_config* config = 
                (struct uart_stm32_config* ) dev->config;

        config->uart_regs->CR1 |= (USART_CR1_TE);
        return 0;
}

static uint8_t stm32_uart_tx_disable(const struct device* dev)
{
        struct uart_stm32_config* config = 
                (struct uart_stm32_config* ) dev->config;

        config->uart_regs->CR1 &= ~(USART_CR1_TE);
        return 0;
}

static uint8_t stm32_uart_is_transmitter_ready(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

        if (data->tx_state == TX_IDLE) {
                return 1;
        }
        return 0;
}

static int stm32_uart_tx(const struct device* dev, const void* buffer, size_t len)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;
        
        if ( (buffer == NULL) && (len == 0) )
                return -1;
        
        data->tx_data = buffer;
        data->tx_data_len = len;
        data->tx_data_cur_byte = 0;
        data->tx_state_flags |= TX_START_TRANSMISSION;

        return 0;
}

static int process_transiever(const struct device* dev);

static int stm32_wait_for_tx_complete(const struct device* dev)
{
        while(!process_transiever(dev));
        return 0;
}

static int stm32_uart_tx_abort(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;

        data->tx_state_flags |= TX_ABORTED;

        return 0;
}

static int process_reciever(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;
        struct uart_stm32_config* config = 
                (struct uart_stm32_config* ) dev->config;

        /* проверка флагов для всех состояний */
        if (data->tx_state_flags & RX_DISABLE) {
                /* сбросить все флаги */
                data->rx_state_flags  = 0;
                data->rx_state = RX_DISABLED;

                config->uart_regs->CR1 &= ~(USART_CR1_RE);
                return 1;
        }

        switch (data->rx_state) {
                case RX_DISABLED:
                        if (data->rx_state_flags & RX_ENABLE) {
                                data->rx_state_flags &= ~(RX_ENABLE);
                                data->rx_state = RX_RECIEVING;

                                config->uart_regs->CR1 |= USART_CR1_RE;
                                return 0;
                        }
                        return 1;

                case RX_WAIT_FOR_READ:
                        if (data->rx_state_flags & RX_READED) {
                                data->rx_state_flags &= ~(RX_READED);
                                data->rx_state = RX_RECIEVING;
                        }
                        return 1;

                case RX_RECIEVING: {
                        if (data->rx_buffer_cur_byte >= data->rx_buffer_size) {
                                /* заполнили весь буфер */
                                data->rx_state = RX_WAIT_FOR_READ;
                                return 1;
                        }

                        if (config->uart_regs->SR & USART_SR_RXNE) {
                                /* Приняли байт */
                                data->rx_buffer[data->rx_buffer_cur_byte] = 
                                        config->uart_regs->DR;
                                data->rx_buffer_cur_byte++;
                        }
                        return 0;
                }
        }

        return 0;
}

static int process_transiever(const struct device* dev)
{
        struct uart_stm32_data* data =
                (struct uart_stm32_data* ) dev->data;
        struct uart_stm32_config* config = 
                (struct uart_stm32_config* ) dev->config;

        switch (data->tx_state) {
                case TX_IDLE:
                        if (data->tx_state_flags & TX_START_TRANSMISSION) {
                                data->tx_state_flags &= ~(TX_START_TRANSMISSION);
                                data->tx_state = TX_TRANSMITTING;
                                return 0;
                        }
                        return 1;

                case TX_TRANSMITTING: {
                        if (data->tx_state_flags & TX_ABORTED) {
                                data->tx_state_flags = 0;
                                data->tx_state = TX_IDLE;
                        }
                        if (data->tx_data_cur_byte >= data->tx_data_len) {
                                data->tx_state = TX_WAIT_TO_TC;
                                return 0;
                        }
                        uint8_t byte_to_tx = 
                                data->tx_data[data->tx_data_cur_byte];
                        data->tx_data_cur_byte++;

                        config->uart_regs->DR = byte_to_tx;

                        data->tx_state = TX_WAIT_TO_TXE;
                        return 0;
                }

                case TX_WAIT_TO_TXE: {
                        if (config->uart_regs->SR & USART_SR_TXE_Msk)
                                data->tx_state = TX_TRANSMITTING;
                        return 0;
                }

                case TX_WAIT_TO_TC: {
                        if (config->uart_regs->SR & USART_SR_TC_Msk) {
                                /* Передача завершена */
                                data->tx_state = TX_IDLE;
                        }
                        return 0;
                }       
        }
        return 0;
}

int uart_stm32_driver_process(const struct device* dev)
{
        int ret1 = process_reciever(dev);
        int ret2 = process_transiever(dev);

        return (ret1 && ret2) ? 1: 0;
}

const struct uart_driver_api uart_stm32_driver_api = {
        .uart_configure = stm32_uart_configure,
        .uart_set_rx_buffer = stm32_uart_set_rx_buffer,
        .uart_rx_enable = stm32_uart_rx_enable,
        .uart_rx_disable = stm32_uart_rx_disable,
        .uart_is_data_available = stm32_uart_is_data_available,
        .uart_rx = stm32_uart_rx,
        .uart_tx_enable = stm32_uart_tx_enable,
        .uart_tx_disable = stm32_uart_tx_disable,
        .uart_is_transmitter_ready = stm32_uart_is_transmitter_ready,
        .uart_tx = stm32_uart_tx,
        .uart_wait_for_tx_complete = stm32_wait_for_tx_complete,
        .uart_tx_abort = stm32_uart_tx_abort
};
