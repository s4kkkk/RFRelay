/**
 * @file 
 * @brief Реализация GPIO-драйвера для stm32f103c8
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <drivers/gpio/gpio.h>
#include <stm32f1xx.h>

#include <drivers/gpio/gpio_stm32f103cx.h>

/**
 * @brief Инициализация драйвера
 */
int gpio_stm32_init_driver(const struct device *dev)
{
        struct gpio_stm32_config* config = (struct gpio_stm32_config* ) dev->config;

        if (config->user_settings->port > 4)
                return -1;

        /* Т.к GPIOA находится по самому маленькому адресу, можем взять GPIO_BASE в качестве
         * базового при вычислении базового адреса конкретного порта 
         */
        GPIO_TypeDef* gpio_regs = (GPIO_TypeDef* ) 
                (GPIOA_BASE + 0x400*config->user_settings->port);
        config->gpio_regs = gpio_regs;

        /* Все порты контролируют по 16 бит */
        config->common.port_pins = 0xFFFF;
        return 0;
}

/**
 * @brief Конфигурирование режима пина
 */
static int gpio_stm32_pins_configure(const struct device* port,
                              gpio_pins_t pins,
                              gpio_flags_t flags)
{
        struct gpio_stm32_config* config = (struct gpio_stm32_config* ) port->config;

        uint32_t crx_value = 0;
        uint8_t pull_flag = 0;

        if (flags & GPIO_INPUT) {
                crx_value = 0b0100;
        }
        else if (flags & GPIO_OUTPUT) {
                crx_value = 0b0011;
        }
        else if ( (flags & GPIO_INPUT_PULLDOWN) || (flags & GPIO_INPUT_PULLUP) ) {
                crx_value = 0b1000;
                pull_flag = 1;
        }

        uint32_t crl_mask = 0, crl_value = 0;
        uint32_t crh_mask = 0, crh_value = 0;
        uint32_t odr_mask = 0, odr_value = 0;
        for (uint8_t i = 0; i < 16; i++) {
                if ( pins & (0b1 << i) ) {
                        /* Пин с номером i присутствует в требуемом наборе */
                        if (i < 8) {
                                /* устаналиванием биты в маске */
                                crl_mask |= ((GPIO_CRL_MODE0_Msk | GPIO_CRL_CNF0_Msk) << (4*i));
                                /* устанавливаем необходимую конфигурацию */
                                crl_value |= crx_value << (4*i);
                        }
                        else {
                                /* устанавливанием биты в маске */
                                crh_mask |= ((GPIO_CRH_MODE8_Msk | 
                                        GPIO_CRH_CNF8_Msk) << ( 4*(i-8) ) );

                                /* устанавливаем необходимую конфигурацию */
                                crh_value |= crx_value << (4*i);
                        }

                        if (pull_flag) {
                                odr_mask |= 0b1<<i;
                                if (flags & GPIO_INPUT_PULLUP) {
                                        odr_value |= 0b1 << i;
                                }
                        }

                }
        }

        /* обнуляем немаскированные поля */
        config->gpio_regs->CRL &= ~(crl_mask);
        config->gpio_regs->CRH &= ~(crh_mask);
        /* записываем значения */
        config->gpio_regs->CRL |= crl_value;
        config->gpio_regs->CRH |= crh_value;

        if (pull_flag) {
                /* необходимо записать в ODR-регистр */
                config->gpio_regs->ODR &= ~(odr_mask);
                config->gpio_regs->ODR |= odr_value;
        }

        return 0;
}

/**
 * @brief Получить текущую конфигурацию пина
 */
static int gpio_stm32_pin_get_config(const struct device* port,
                              gpio_pin_t pin,
                              gpio_flags_t* flags)
{
        struct gpio_stm32_config* config = (struct gpio_stm32_config* ) port->config;

        uint32_t crx_value = 0;
        if (pin < 8) {
                crx_value = config->gpio_regs->CRL;
                crx_value = crx_value >> pin*4;
        }
        else {
                crx_value = config->gpio_regs->CRH;
                crx_value = crx_value >> pin*4;
        }

        if (crx_value == 0b0100) {
                *flags = GPIO_INPUT;
        }
        else if (crx_value == 0b0011) {
                *flags = GPIO_OUTPUT;
        }
        else if (crx_value == 0b1000) {
                if ( config->gpio_regs->ODR & (0b1<<pin)) {
                        *flags = GPIO_INPUT_PULLUP;
                }
                else {
                        *flags = GPIO_INPUT_PULLDOWN;
                }
        }

        return 0;
}

/**
 * @brief Чтение состояния пинов
 */
static int gpio_stm32_pins_read_raw(const struct device* port, gpio_pins_values_t* values)
{
        struct gpio_stm32_config* config = (struct gpio_stm32_config* ) port->config;

        *values = config->gpio_regs->IDR;

        return 0;
}

/**
 * @brief Установка пинов
 */
static int gpio_stm32_pins_set_raw(const struct device* port, gpio_pins_t pins)
{
        struct gpio_stm32_config* config = (struct gpio_stm32_config* ) port->config;

        config->gpio_regs->ODR |= (config->common.port_pins & pins);

        return 0;
}

/**
 * @brief Сброс пинов
 */
static int gpio_stm32_pins_clear_raw(const struct device* port, gpio_pins_t pins)
{
        struct gpio_stm32_config* config = (struct gpio_stm32_config* ) port->config;

        config->gpio_regs->ODR &= ~(config->common.port_pins & pins);

        return 0;
}

const struct gpio_driver_api gpio_stm32_driver_api = {
        .pins_configure = gpio_stm32_pins_configure,
        .pin_get_config = gpio_stm32_pin_get_config,
        .pins_read_raw = gpio_stm32_pins_read_raw,
        .pins_set_raw = gpio_stm32_pins_set_raw,
        .pins_clear_raw = gpio_stm32_pins_clear_raw
};
