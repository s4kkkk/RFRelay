/**
 * @file 
 * @brief Реализация функций API подсистемы GPIO
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <drivers/gpio.h>

int gpio_pin_configure(const struct device* port,
                                     gpio_pin_t pin,
                                     gpio_flags_t flags)
{
        struct gpio_driver_config* config = (struct gpio_driver_config* ) port->config;
        if ( (~(config->port_pins)) & (0b1<<pin) ) {
                /* Пин не поддерживается */
                return -1;
        }

        const struct gpio_driver_api* api = 
                (const struct gpio_driver_api* )port->api;

        if (api->pins_configure == NULL)
                return -1;

        gpio_pins_t pins = (1U << pin);

        return api->pins_configure(port, pins, flags);
}

int gpio_pin_get_config(const struct device* port,
                                      gpio_pin_t pin,
                                      gpio_flags_t* flags)
{
        struct gpio_driver_config* config = (struct gpio_driver_config* ) port->config;
        if ( (~(config->port_pins)) & (0b1<<pin) ) {
                /* Пин не поддерживается */
                return -1;
        }

        const struct gpio_driver_api* api = 
                (const struct gpio_driver_api* )port->api;

        if (api->pin_get_config == NULL)
                return -1;

        return api->pin_get_config(port, pin, flags);
}

int gpio_pin_set(const struct device* port,
                               gpio_pin_t pin,
                               uint8_t value)
{
        struct gpio_driver_config* config = (struct gpio_driver_config* ) port->config;
        if ( (~(config->port_pins)) & (0b1<<pin) ) {
                /* Пин не поддерживается */
                return -1;
        }

        const struct gpio_driver_api* api = 
                (const struct gpio_driver_api* )port->api;
        int ret;

        if (value == 0) {
                if (api->pins_clear_raw == 0)
                        return -1;

                ret = api->pins_clear_raw(port, (1U<<pin));
        }
        else {
                if (api->pins_set_raw == 0)
                        return -1;

                ret = api->pins_set_raw(port, (1U<<pin));
        }

        return ret;
}

int gpio_pin_read(const struct device* port, gpio_pin_t pin)
{
        struct gpio_driver_config* config = (struct gpio_driver_config* ) port->config;
        if ( (~(config->port_pins)) & (0b1<<pin) ) {
                /* Пин не поддерживается */
                return -1;
        }

        const struct gpio_driver_api* api = 
                (const struct gpio_driver_api* )port->api;
        int ret;

        gpio_pins_values_t gpio_pins_values;

        if (api->pins_read_raw == NULL)
                return -1;

        ret = api->pins_read_raw(port, &gpio_pins_values);
        if (ret != 0)
                return -1;

        return ((gpio_pins_values & (1U << pin))) ? 1 : 0;
}

int gpio_pin_toggle(const struct device* port, gpio_pin_t pin)
{
        struct gpio_driver_config* config = (struct gpio_driver_config* ) port->config;
        if ( (~(config->port_pins)) & (0b1<<pin) ) {
                /* Пин не поддерживается */
                return -1;
        }

        int ret;
        ret = gpio_pin_read(port, pin);
        if (ret < 0)
                return ret;
        else if (ret == 0) {
                ret = gpio_pin_set(port, pin, 1);
        }
        else {
                ret = gpio_pin_set(port, pin, 0);
        }

        return ret;
}
