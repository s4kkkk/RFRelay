#include <device.h>
#include <printk.h>
#include <drivers/gpio/gpio.h>
#include <drivers/uart/uart.h>
#include <drivers/uart/uart_stm32f103cx.h>
#include <drivers/modules/nrf24l01/nrf24l01.h>

#include "config.h"
#include "init.h"
#include "conn_fsm.h"
#include "indicator_fsm.h"
#include "timer.h"


struct work_data_t {
        enum {
                INIT,
                WAIT_FOR_RELAY_SETTING,
                RELAY_SETTED,
                WAIT_FOR_RELAY_RESETTING,
                RELAY_RESETTED,
        } state;

        const struct device* switch_gpio_controller;
        const struct device* relay_gpio_controller;

        struct timer_t relay_timer;


} work_data;

void work_init(struct work_data_t* work_data)
{
        const struct device* dev = NULL;

        dev = device_get_by_name("gpio_switch");

        if (dev == NULL) {
                DEBUG("Паника: не найден драйвер gpio выключателя\n");
                while(1);
        }

        work_data->switch_gpio_controller = dev;

        gpio_pin_configure(work_data->switch_gpio_controller, CONFIG_GPIO_RELAY_SET_PIN_NUM, GPIO_OUTPUT);
        gpio_pin_configure(work_data->switch_gpio_controller, CONFIG_GPIO_RELAY_RST_PIN_NUM, GPIO_OUTPUT);

        gpio_pin_set(work_data->switch_gpio_controller, CONFIG_GPIO_RELAY_SET_PIN_NUM, 0);
        gpio_pin_set(work_data->switch_gpio_controller, CONFIG_GPIO_RELAY_RST_PIN_NUM, 0);


        dev = device_get_by_name("gpio_relay");

        if (dev == NULL) {
                DEBUG("Паника: не найден драйвер gpio реле\n");
                while(1);
        }

        work_data->relay_gpio_controller = dev;

        work_data->state = INIT;
        return;
}

static inline void work_default(struct work_data_t* work_data)
{
        if (!conn_fsm_need_self_work(&conn_fsm_data)) {

                if (gpio_pin_read(work_data->switch_gpio_controller, CONFIG_GPIO_SWITCH_PIN_NUM) && 
                    conn_fsm_is_uu_online_power(&conn_fsm_data)) {

                        /* Реле должно быть включено*/
                        if (work_data->state != RELAY_SETTED) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_SET_PIN_NUM, 1);
                                DEBUG("Установка реле...\n");

                                setup_timer(&work_data->relay_timer, CONFIG_RELAY_WAIT_TIME);
                                reset_timer(&work_data->relay_timer);

                                work_data->state = WAIT_FOR_RELAY_SETTING;
                                return;
                        }
                }
                else {

                        /* Реле должно быть выключено */
                        if (work_data->state != RELAY_RESETTED) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_RST_PIN_NUM, 1);
                                DEBUG("Сброс реле...\n");

                                setup_timer(&work_data->relay_timer, CONFIG_RELAY_WAIT_TIME);
                                reset_timer(&work_data->relay_timer);

                                work_data->state = WAIT_FOR_RELAY_RESETTING;
                                return;
                        }

                }
        }

        else {
                /* Состояние реле определяется только выключателем */
                if (gpio_pin_read(work_data->switch_gpio_controller, CONFIG_GPIO_SWITCH_PIN_NUM)) {

                        /* Реле должно быть включено*/
                        if (work_data->state != RELAY_SETTED) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_SET_PIN_NUM, 1);
                                DEBUG("Установка реле...\n");

                                setup_timer(&work_data->relay_timer, CONFIG_RELAY_WAIT_TIME);
                                reset_timer(&work_data->relay_timer);

                                work_data->state = WAIT_FOR_RELAY_SETTING;
                                return;
                        }
                }
                else {

                        /* Реле должно быть выключено */
                        if (work_data->state != RELAY_RESETTED) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_RST_PIN_NUM, 1);
                                DEBUG("Сброс реле...\n");

                                setup_timer(&work_data->relay_timer, CONFIG_RELAY_WAIT_TIME);
                                reset_timer(&work_data->relay_timer);

                                work_data->state = WAIT_FOR_RELAY_RESETTING;
                                return;
                        }

                }
        }

        return;
}

void work(struct work_data_t* work_data)
{

        switch (work_data->state) {

                case WAIT_FOR_RELAY_SETTING: {
                        if (is_timer_elapsed(&work_data->relay_timer)) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_SET_PIN_NUM, 0);
                                work_data->state = RELAY_SETTED;
                        }
                        break;
                }

                case WAIT_FOR_RELAY_RESETTING: {
                        if (is_timer_elapsed(&work_data->relay_timer)) {
                                gpio_pin_set(work_data->relay_gpio_controller, CONFIG_GPIO_RELAY_RST_PIN_NUM, 0);
                                work_data->state = RELAY_RESETTED;
                        }
                        break;
                }

                default: {
                        work_default(work_data);
                        break;
                }

        }

        return;
}

int main(void)
{

        int ret;

        ret = init();
        if (ret != 0) {
                DEBUG("Паника: ошибка инициализации аппаратных подсистем\n");
                while(1);
        }

        work_init(&work_data);
        indicator_fsm_init(&indicator_fsm_data);
        DEBUG("Инициализация завершена...\n");

        /* Суперцикл */
        while(1) {
                work(&work_data);
                conn_fsm_work(&conn_fsm_data);
                indicator_fsm_work(&indicator_fsm_data);
        }

        return 0;
}
