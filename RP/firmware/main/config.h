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
 * Номер GPIO-контроллера, который будет использоваться для работы с выключателем
 * GPIOA: 0
 * GPIOB: 1
 * ...
 */
#define CONFIG_GPIO_SWITCH_CONTROLLER_NUM 1

/** Номер GPIO-пина, к которому подключен выключатель */
#define CONFIG_GPIO_SWITCH_PIN_NUM 0

/**
 * Номер GPIO-контроллера, который будет использоваться для работы с реле
 * GPIOA: 0
 * GPIOB: 1
 */
#define CONFIG_GPIO_RELAY_CONTROLLER_NUM 1

/** Номер GPIO-пина, к которому подключен пин сброса реле */
#define CONFIG_GPIO_RELAY_SET_PIN_NUM 1

/** Номер GPIO-пина, к которому подключен пин установки реле */
#define CONFIG_GPIO_RELAY_RST_PIN_NUM 2

/** Время в мс, в течение которого РП будет ждать переключения реле */
#define CONFIG_RELAY_WAIT_TIME 100

/** частота тактирования системного таймера */
#define CONFIG_SYSTICK_CLOCK 1000000UL

/** MAC источника */
#define CONFIG_RADIO_RX_MAC {0xD7, 0xD7, 0xD7, 0xD7, 0xD7}

/** MAC назначения */
#define CONFIG_RADIO_TX_MAC {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}

/** Размер полезной нагрузки в байтах */
#define CONFIG_RADIO_PAYLOAD_LEN 3

/** Время в мс, в течение которого на канале будет ожидаться beacon-пакет */
#define CONFIG_BEACON_WAIT_TIME 250

/** Время в мс, по истечении которого будет отправлен очередной запрос к УУ */
#define CONFIG_STATUS_REQ_TIME 10000

/** Время в мс. Если в течение этого времени не будет осуществлен ни один сеанс связи,
 * то соединение будет считаться разорванным, а РП перейдет в режим поиска нового канала
 */
#define CONFIG_STANDBY_TIME 30000

/** Время в мс, по истечении которого РП перейдет в самостоятельный режим (будет включать реле только по переключателю,
 * в независимости от УУ).
 */
#define CONFIG_SELF_WORK_TIME 200000

/** Время в мс, в течение которого будет ожидаться ответ от УУ */
#define CONFIG_ANSWER_WAIT_TIME 200

/** Кол-во пакетов, которые будут отправляться к УУ в качестве ответа на beacon.
 * Используется для повышения стабильности поиска частоты
 */
#define CONFIG_BEACON_ANS_PACKETS_COUNT 5

#endif // CONFIG_H
