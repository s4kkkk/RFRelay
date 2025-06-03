#include <device.h>
#include <printk.h>
#include <drivers/gpio/gpio.h>
#include <drivers/uart/uart.h>
#include <drivers/uart/uart_stm32f103cx.h>
#include <drivers/modules/nrf24l01/nrf24l01.h>

#include "config.h"
#include "init.h"
#include "conn_fsm.h"

struct work_data_t {
        enum {
                LED_ON,
                LED_OFF,
        } state;

        const struct device* switch_gpio_controller;
        const struct device* relay_gpio_controller;

} work_data;

void work_init(struct work_data_t* work_data)
{
        const struct device* dev = NULL;

        work_data->state = LED_OFF;

        dev = device_get_by_name("gpio_switch");

        if (dev == NULL) {
                DEBUG("Паника: не найден драйвер gpio выключателя\n");
                while(1);
        }

        work_data->switch_gpio_controller = dev;

        dev = device_get_by_name("gpio_relay");

        if (dev == NULL) {
                DEBUG("Паника: не найден драйвер gpio реле\n");
                while(1);
        }

        work_data->relay_gpio_controller = dev;
        return;
}

void work(struct work_data_t* work_data)
{
        if (!conn_fsm_need_self_work(&conn_fsm_data)) {

                if (gpio_pin_read(work_data->switch_gpio_controller, CONFIG_GPIO_SWITCH_PIN_NUM) && 
                    conn_fsm_is_uu_online_power(&conn_fsm_data)) {
                        /* Реле должно быть выключено*/
                        if (!gpio_pin_read(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM)) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM, 1);
                        }
                }
                else {
                        /* Реле должно быть выключено */
                        if (gpio_pin_read(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM)) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM, 0);

                        }
                }
        }

        else {
                /* Состояние реле определяется только выключателем */
                if (gpio_pin_read(work_data->switch_gpio_controller, CONFIG_GPIO_SWITCH_PIN_NUM)) {
                        /* Реле должно быть выключено*/
                        if (!gpio_pin_read(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM)) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM, 1);
                        }

                }
                else {
                        /* Реле должно быть выключено */
                        if (gpio_pin_read(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM)) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_PIN_NUM, 0);

                        }
                }
        }

        return;
}

int main(void)
{
        DEBUG("Инициализация...\n");

        int ret;

        ret = init();
        if (ret != 0) {
                DEBUG("Паника: ошибка инициализации аппаратных подсистем\n");
                while(1);
        }

        /* Суперцикл */

        while(1) {
                work(&work_data);
                conn_fsm_work(&conn_fsm_data);
        }

        return 0;
}
