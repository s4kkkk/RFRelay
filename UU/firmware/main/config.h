/**
 * @file 
 * @brief Конфигурация прошивки
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "systick.h"

#define DEBUG_ON

#ifdef DEBUG_ON
#define DEBUG(fmt, ...) \
        printk("[%d] " fmt, (uint32_t ) afterboot_time_ms(), ##__VA_ARGS__)
#else
#define DEBUG(...)
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

/**
 * Номер GPIO-контроллера, который будет использоваться для работы с оптопарой
 * GPIOA: 0
 * GPIOB: 1
 * ...
 */
#define CONFIG_GPIO_OPTO_CONTROLLER_NUM 0

/** Номер GPIO-пина, к которому подлкючена оптопара */
#define CONFIG_GPIO_OPTO_PIN_NUM 2

/**
 * Номер GPIO-контроллера, который будет использоваться для индикации светодиодами
 * GPIOA: 0
 * GPIOB: 1
 * ...
 */
#define CONFIG_GPIO_LED_CONTROLLER_NUM 1

/** Номер GPIO-пина, к которому подключен красный светодиод */
#define CONFIG_GPIO_RED_LED_PIN 13

/** Номер GPIO-пина, к которому подключен зеленый светодиод */
#define CONFIG_GPIO_GREEN_LED_PIN 15

/** частота тактирования системного таймера */
#define CONFIG_SYSTICK_CLOCK 1000000UL

/** MAC источника */
#define CONFIG_RADIO_RX_MAC {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}

/** MAC назначения */
#define CONFIG_RADIO_TX_MAC {0xD7, 0xD7, 0xD7, 0xD7, 0xD7}


/** Размер полезной нагрузки в байтах */
#define CONFIG_RADIO_PAYLOAD_LEN 3

/** Время в мс, в течение которого УУ будет слать beacon-пакет на одном канале */ 
#define CONFIG_CH_FND_TIME 1000

/** Время в мс, в течение которого УУ будет ждать ответа на beacon-пакет */
#define CONFIG_BEACON_ANS_TIME 50

/** Время в мс. Если в течение этого времени не будет осуществлена ни одна передача,
 * то соединение будет считаться разорванным
 */
#define CONFIG_STANDBY_TIME 15000

#endif // CONFIG_H
