/**
 * @file 
 * @brief Реализация конечного автомата, обеспечивающего индикацию статуса
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "indicator_fsm.h"

#include <stddef.h>

#include <device.h>
#include <printk.h>
#include <drivers/gpio/gpio.h>

#include "config.h"
#include "timer.h"

#define BLINK_DELAY 50
#define SMALL_DELAY 100
#define LARGE_DELAY 1000

struct indicator_fsm_data_t {
        const struct device* led_gpio;

        enum indicator_status_t status;


} indicator_fsm_data;

void indicator_fsm_init(struct indicator_fsm_data_t* indicator_fsm_data)
{
        const struct device* dev = NULL;
        dev = device_get_by_name("gpio_led");

        if (dev == NULL) {
                DEBUG("Паника: не найден контроллер gpio_led\n");
                while (1);
        }
        indicator_fsm_data->led_gpio = dev;
        
        gpio_pin_configure(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, GPIO_OUTPUT);
        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 0);

        gpio_pin_configure(indicator_fsm_data->led_gpio, CONFIG_GPIO_GREEN_LED_PIN, GPIO_OUTPUT);
        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_GREEN_LED_PIN, 0);

        indicator_fsm_data->status = INDICATOR_FREQ_FINDING;
        return;
}

static inline void indicator_fsm_indicator_freq_finding(struct indicator_fsm_data_t* indicator_fsm_data)
{
        static uint8_t fsm_stage = 0;
        static struct timer_t timer;

        switch (fsm_stage) {
                case 0: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 1);
                        setup_timer(&timer, BLINK_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 1;
                        break;
                }

                case 1: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 2;
                        }
                        break;
                }

                case 2: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 0);
                        setup_timer(&timer, LARGE_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 3;
                        break;
                }

                case 3: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 0;
                        }
                        break;
                }
        }
        return;
}

static inline void indicator_fsm_indicator_conn_established(struct indicator_fsm_data_t* indicator_fsm_data)
{
        static uint8_t fsm_stage = 0;
        static struct timer_t timer;

        switch (fsm_stage) {
                case 0: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_GREEN_LED_PIN, 1);
                        setup_timer(&timer, BLINK_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 1;
                        break;
                }

                case 1: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 2;
                        }
                        break;
                }

                case 2: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_GREEN_LED_PIN, 0);
                        setup_timer(&timer, LARGE_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 3;
                        break;
                }

                case 3: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 0;
                        }
                        break;
                }
        }
        return;
}

static inline void indicator_fsm_indicator_self_work(struct indicator_fsm_data_t* indicator_fsm_data)
{
        static uint8_t fsm_stage = 0;
        static struct timer_t timer;

        switch (fsm_stage) {
                case 0: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 1);
                        setup_timer(&timer, BLINK_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 1;
                        break;
                }

                case 1: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 2;
                        }
                        break;
                }

                case 2: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 0);
                        setup_timer(&timer, SMALL_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 3;
                        break;
                }

                case 3: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 4;
                        }
                        break;
                }

                case 4: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 1);
                        setup_timer(&timer, BLINK_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 5;
                        break;
                }

                case 5: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 6;
                        }
                        break;
                }

                case 6: {
                        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 0);
                        setup_timer(&timer, LARGE_DELAY);
                        reset_timer(&timer);
                        fsm_stage = 7;
                        break;
                }

                case 7: {
                        if (is_timer_elapsed(&timer)) {
                                fsm_stage = 0;
                        }
                }


        }
        return;

}

void indicator_fsm_work(struct indicator_fsm_data_t* indicator_fsm_data)
{

        switch (indicator_fsm_data->status) {
                case INDICATOR_FREQ_FINDING: {
                        indicator_fsm_indicator_freq_finding(indicator_fsm_data);
                        break;
                }

                case INDICATOR_CONN_ESTABLISHED: {
                        indicator_fsm_indicator_conn_established(indicator_fsm_data);
                        break;
                }

                case INDICATOR_SELF_WORK: {
                        indicator_fsm_indicator_self_work(indicator_fsm_data);
                        break;
                }
        }

        return;
}

void indicator_set_status(struct indicator_fsm_data_t* indicator_fsm_data, enum indicator_status_t status)
{
        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_RED_LED_PIN, 0);
        gpio_pin_set(indicator_fsm_data->led_gpio, CONFIG_GPIO_GREEN_LED_PIN, 0);

        indicator_fsm_data->status = status;
        return;
}
