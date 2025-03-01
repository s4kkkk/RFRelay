/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
*
* Modifications in 2025 by Stanislav Timoshko <s4kkkk@mail.ru>
*
* -----------------------------------------------------------------------------
*/

#include <drivers/modules/nrf24l01/nrf24l01_soft_driver.h>

const struct device* nrf24l01_soft_driver_instance;

void nrf24_setupPins()
{
        return;
}

void nrf24_ce_digitalWrite(uint8_t state)
{
        struct nrf24l01_soft_driver_config* config = 
                (struct nrf24l01_soft_driver_config* ) nrf24l01_soft_driver_instance->config;
        
        gpio_pin_set(
                config->user_settings->gpio_controller,
                config->user_settings->pins.ce_pin,
                state
        );
}

void nrf24_csn_digitalWrite(uint8_t state)
{
        struct nrf24l01_soft_driver_config* config = 
                (struct nrf24l01_soft_driver_config* ) nrf24l01_soft_driver_instance->config;
        
        gpio_pin_set(
                config->user_settings->gpio_controller,
                config->user_settings->pins.cs_pin,
                state
        );
}

void nrf24_sck_digitalWrite(uint8_t state)
{
        struct nrf24l01_soft_driver_config* config = 
                (struct nrf24l01_soft_driver_config* ) nrf24l01_soft_driver_instance->config;
        
        gpio_pin_set(
                config->user_settings->gpio_controller,
                config->user_settings->pins.sck_pin,
                state
        );
}

void nrf24_mosi_digitalWrite(uint8_t state)
{
        struct nrf24l01_soft_driver_config* config = 
                (struct nrf24l01_soft_driver_config* ) nrf24l01_soft_driver_instance->config;
        
        gpio_pin_set(
                config->user_settings->gpio_controller,
                config->user_settings->pins.mosi_pin,
                state
        );
}
uint8_t nrf24_miso_digitalRead()
{
        struct nrf24l01_soft_driver_config* config = 
                (struct nrf24l01_soft_driver_config* ) nrf24l01_soft_driver_instance->config;

        return gpio_pin_read(config->user_settings->gpio_controller,
                             config->user_settings->pins.miso_pin);
}
