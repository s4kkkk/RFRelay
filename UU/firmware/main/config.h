/**
 * @file 
 * @brief Конфигурация прошивки
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG_ON

#ifdef DEBUG_ON
#define DEBUG(msg) printk(msg)
#else
#define DEBUG(msg)
#endif


/**
 * Номер UART-контроллера.
 * USART1: 1
 * USART2: 2
 * ...
 */
#define CONFIG_USART_CONTROLLER_NUM 1

/** Частота тактирования USART-контроллера в Гц */
#define CONFIG_USART_CLOCK 8000000

/** Бодрейт USART */
#define CONFIG_USART_BAUDRATE 9600

/**
 * Номер GPIO-контроллера, который будет использоваться для работы с nrf24l01
 * GPIOA: 0
 * GPIOB: 1
 * ...
 */
#define CONFIG_GPIO_RADIO_CONTROLLER_NUM 0

/**
 * @name Пины GPIO, к которым подключен nrf24l01
 * @{
 */

#define CONFIG_RADIO_CE_PIN 4
#define CONFIG_RADIO_CS_PIN 5
#define CONFIG_RADIO_SCK_PIN 6
#define CONFIG_RADIO_MOSI_PIN 7
#define CONFIG_RADIO_MISO_PIN 3

/** @} */

/** частота тактирования системного таймера */
#define SYSTICK_CLOCK 1000000UL

/** MAC источника */
#define CONFIG_RADIO_RX_MAC {0xD7, 0xD7, 0xD7, 0xD7, 0xD7}

/** MAC назначения */
#define CONFIG_RADIO_TX_MAC {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}

/** Размер полезной нагрузки в байтах */
#define CONFIG_RADIO_PAYLOAD_LEN 3

#endif // CONFIG_H
