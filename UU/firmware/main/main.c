#include <device.h>
#include <printk.h>
#include <drivers/gpio/gpio.h>
#include <drivers/uart/uart.h>
#include <drivers/uart/uart_stm32f103cx.h>
#include <drivers/modules/nrf24l01/nrf24l01.h>

#include "config.h"
#include "init.h"
#include "timer.h"
#include "packets.h"

struct main_uu_data_t {
        enum {
        STANDBY,
        WAIT_TO_TX_END,
        FREQ_FINDING,
        FREQ_FINDING_CH_PROC,
        FREQ_FINDING_BEACON_SEND,
        FREQ_FINDING_WAIT_ANS,
        } state;

        const struct device* radio_nrf;

        const struct device* gpio_opto_controller;

        /** Таймер поиска на канале */
        struct timer_t ch_finding_timer;

        /** Таймер ожидания ответа на beacon-пакет */
        struct timer_t beacon_ans_timer;

        /** Standby-таймер. Нужен для определения момента потери соединения */
        struct  timer_t standby_timer;

        /** текущий уровень сигнала с оптопары */
        int prev_opto_status;


} main_uu_data;

void main_uu_fsm_init(struct main_uu_data_t* main_uu_data)
{
        main_uu_data->state = FREQ_FINDING;

        const struct device* dev = NULL;

        dev = device_get_by_name("nrf24l01_1");
        if (dev == NULL) {
                DEBUG("Паника: не найден модуль nrf24l01\n");
                while(1);
        }
        main_uu_data->radio_nrf = dev;

        dev = device_get_by_name("gpio_opto");
        if (dev == NULL) {
                DEBUG("Паника: не найден драйвер gpio_opto\n");
                while(1);
        }
        main_uu_data->gpio_opto_controller = dev;

        main_uu_data->prev_opto_status = 0;


        return;
}

static inline void main_uu_fsm_freq_finding(struct main_uu_data_t* main_uu_data)
{
        uint8_t current_channel = nrf24l01_get_channel(main_uu_data->radio_nrf);
        if (current_channel < 127) {
                current_channel++;
                nrf24l01_set_channel(main_uu_data->radio_nrf, current_channel);
        }
        else {
                current_channel = 0;
                nrf24l01_set_channel(main_uu_data->radio_nrf, current_channel);
        }

        main_uu_data->state = FREQ_FINDING_CH_PROC;
        setup_timer(&main_uu_data->ch_finding_timer, CONFIG_CH_FND_TIME);
        reset_timer(&main_uu_data->ch_finding_timer);
        DEBUG("Поиск рабочего канала: %d / 127\n", current_channel);
        return;
}

static inline void main_uu_fsm_freq_finding_ch_proc(struct main_uu_data_t* main_uu_data)
{
        if (is_timer_elapsed(&main_uu_data->ch_finding_timer)) {
                main_uu_data->state = FREQ_FINDING;
                return;
        }

        struct rp_packet beacon_packet = {
                .packet_type = PACKET_TYPE_BEACON,
                .flags = FLAG_NONE,
        };
        packet_insert_checksum(&beacon_packet);

        /* Начало передачи beacon-пакета */
        nrf24l01_send(main_uu_data->radio_nrf, (const uint8_t* ) (&beacon_packet));

        main_uu_data->state = FREQ_FINDING_BEACON_SEND;
        return;
}

static inline void main_uu_fsm_freq_finding_wait_ans(struct main_uu_data_t* main_uu_data)
{
        if (is_timer_elapsed(&main_uu_data->beacon_ans_timer)) {

                main_uu_data->state = FREQ_FINDING_CH_PROC;
                return;
        }

        struct rp_packet recieved_packet;

        if (nrf24l01_is_data_ready(main_uu_data->radio_nrf)) {
                nrf24l01_get_data(main_uu_data->radio_nrf, (uint8_t* ) &recieved_packet);

                if (packet_has_correct_checksum(&recieved_packet)) {
                        setup_timer(&main_uu_data->standby_timer, CONFIG_STANDBY_TIME);
                        reset_timer(&main_uu_data->standby_timer);
                        DEBUG("На канале %d получен ответ. Соединение установлено!\n",
                              nrf24l01_get_channel(main_uu_data->radio_nrf));

                        main_uu_data->state = STANDBY;
                }

        }

        return;
}

static inline int8_t opto_get_line_status(void)
{
        return 1;
}

static inline void create_and_send_status_packet(struct main_uu_data_t* main_uu_data)
{
        int opto_pin = gpio_pin_read(main_uu_data->gpio_opto_controller, CONFIG_GPIO_OPTO_PIN_NUM);
        main_uu_data->prev_opto_status = opto_pin;

        struct rp_packet status_packet = {
                .packet_type = PACKET_TYPE_STATUS,
        };
        
        if (opto_pin) {
                status_packet.flags = FLAG_ON_LINE_POWER;
        }
        else {
                status_packet.flags = FLAG_OFF_LINE;
        }

        packet_insert_checksum(&status_packet);

        nrf24l01_send(main_uu_data->radio_nrf, (const uint8_t* ) &status_packet);

        return;
}

static inline void main_uu_fsm_standby(struct main_uu_data_t* main_uu_data)
{
        int opto_pin = gpio_pin_read(main_uu_data->gpio_opto_controller, CONFIG_GPIO_OPTO_PIN_NUM);
        if (opto_pin != main_uu_data->prev_opto_status) {
                create_and_send_status_packet(main_uu_data);
                main_uu_data->state = WAIT_TO_TX_END;
                return;
        }

        struct rp_packet recieved_packet;

        if (nrf24l01_is_data_ready(main_uu_data->radio_nrf)) {
                nrf24l01_get_data(main_uu_data->radio_nrf, (uint8_t* ) &recieved_packet);

                if (packet_has_correct_checksum(&recieved_packet)) {
                        reset_timer(&main_uu_data->standby_timer);
                        if (recieved_packet.packet_type == PACKET_TYPE_REQUEST_STATUS) {
                                create_and_send_status_packet(main_uu_data);
                                main_uu_data->state = WAIT_TO_TX_END;
                        }

                }

        }
        else if (is_timer_elapsed(&main_uu_data->standby_timer)) {
                DEBUG("Истек STANDBY-таймер. Начинаю поиск частоты\n");
                main_uu_data->state = FREQ_FINDING;
        }

        return;
}


void main_uu_fsm_work(struct main_uu_data_t* main_uu_data)
{
        switch (main_uu_data->state) {
                case FREQ_FINDING: {
                        main_uu_fsm_freq_finding(main_uu_data);
                        break;
                }

                case FREQ_FINDING_CH_PROC: {
                        main_uu_fsm_freq_finding_ch_proc(main_uu_data);
                        break;
                }

                case FREQ_FINDING_BEACON_SEND: {
                        if (!nrf24l01_is_sending(main_uu_data->radio_nrf)) {
                                setup_timer(&main_uu_data->beacon_ans_timer, CONFIG_BEACON_ANS_TIME);
                                reset_timer(&main_uu_data->beacon_ans_timer);
                                main_uu_data->state = FREQ_FINDING_WAIT_ANS;
                        }
                        break;
                }

                case FREQ_FINDING_WAIT_ANS: {
                        main_uu_fsm_freq_finding_wait_ans(main_uu_data);
                        break;
                }

                case STANDBY: {
                        main_uu_fsm_standby(main_uu_data);
                        break;
                }

                case WAIT_TO_TX_END: {
                        if (!nrf24l01_is_sending(main_uu_data->radio_nrf)) {
                                main_uu_data->state = STANDBY;
                        }
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

        DEBUG("Инициализация завершена...\n");

        main_uu_fsm_init(&main_uu_data);

        /* Суперцикл */
        while(1) {
                main_uu_fsm_work(&main_uu_data);
        }

        return 0;
}
