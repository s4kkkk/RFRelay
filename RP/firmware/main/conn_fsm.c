/**
 * @file 
 * @brief Реализация конечного автомата, поддерживающего соеднинение с УУ
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "conn_fsm.h"

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


struct conn_fsm_data_t {
        enum {
                ON_LINE_POWER,
                OFF_LINE_POWER
        } uu_status;

        enum {
                STANDBY,
                WAIT_TX_COMPLETE,
                FREQ_FINDING,
                FREQ_FINDING_CH_PROC,
                FREQ_FINDING_WAIT_TX_COMPLETE,
                WAIT_FOR_STATUS_PACKET
        } state;

        const struct device* radio_nrf;

        struct timer_t beacon_timer;
        struct timer_t standby_timer;
        struct timer_t status_req_timer;
        struct timer_t status_answer_timer;

} conn_fsm_data;

void conn_fsm_init(struct conn_fsm_data_t* conn_fsm_data)
{
        conn_fsm_data->uu_status = OFF_LINE_POWER;
        conn_fsm_data->state = FREQ_FINDING;

        const struct device* dev = NULL;

        dev = device_get_by_name("nrf24l01_1");
        if (dev == NULL) {
                DEBUG("Паника: не найден модуль nrf24l01\n");
                while(1);
        }
        return;
}

static inline void conn_fsm_freq_finding(struct conn_fsm_data_t* conn_fsm_data)
{
        uint8_t current_channel = nrf24l01_get_channel(conn_fsm_data->radio_nrf);
        if (current_channel < 127) {
                nrf24l01_set_channel(conn_fsm_data->radio_nrf, current_channel+1);
        }
        else {
                nrf24l01_set_channel(conn_fsm_data->radio_nrf, 0);
        }

        conn_fsm_data->state = FREQ_FINDING_CH_PROC;
        setup_timer(&conn_fsm_data->beacon_timer, CONFIG_BEACON_WAIT_TIME);
        reset_timer(&conn_fsm_data->beacon_timer);
        return;
}

static inline void create_and_send_beacon_answer(struct conn_fsm_data_t* conn_fsm_data)
{
        struct rp_packet beacon_answer = {
                .packet_type = PACKET_TYPE_SERVICE,
                .flags = FLAG_NONE
        };

        packet_insert_checksum(&beacon_answer);

        nrf24l01_send(conn_fsm_data->radio_nrf, (const uint8_t* ) &beacon_answer);

        return;
}

static inline void conn_fsm_freq_finding_ch_proc(struct conn_fsm_data_t* conn_fsm_data)
{
        if (is_timer_elapsed(&conn_fsm_data->beacon_timer)) {
                conn_fsm_data->state = FREQ_FINDING;
                return;
        }

        struct rp_packet recieved_packet;

        if (nrf24l01_is_data_ready(conn_fsm_data->radio_nrf)) {
                nrf24l01_get_data(conn_fsm_data->radio_nrf, (uint8_t* ) &recieved_packet);

                if (packet_validate_checksum(&recieved_packet)) {
                        if (recieved_packet.packet_type == PACKET_TYPE_BEACON) {
                                create_and_send_beacon_answer(conn_fsm_data);
                                DEBUG("На канале %d пойман beacon-пакет!\n",
                                      nrf24l01_get_channel(conn_fsm_data->radio_nrf));

                                conn_fsm_data->state = FREQ_FINDING_WAIT_TX_COMPLETE;
                        }

                }

        }

        return;
}

static inline void create_and_send_status_req_packet(struct conn_fsm_data_t* conn_fsm_data)
{
        struct rp_packet beacon_answer = {
                .packet_type = PACKET_TYPE_REQUEST_STATUS,
                .flags = FLAG_NONE
        };

        packet_insert_checksum(&beacon_answer);

        nrf24l01_send(conn_fsm_data->radio_nrf, (const uint8_t* ) &beacon_answer);

        return;
}

static inline void conn_fsm_standby(struct conn_fsm_data_t* conn_fsm_data)
{

        struct rp_packet recieved_packet;

        if (nrf24l01_is_data_ready(conn_fsm_data->radio_nrf)) {
                nrf24l01_get_data(conn_fsm_data->radio_nrf, (uint8_t* ) &recieved_packet);

                if (packet_validate_checksum(&recieved_packet)) {
                        if (recieved_packet.packet_type == PACKET_TYPE_STATUS) {
                                conn_fsm_data->uu_status = (recieved_packet.flags == FLAG_ON_LINE_POWER) ?
                                        ON_LINE_POWER : OFF_LINE_POWER;
                                reset_timer(&conn_fsm_data->standby_timer);
                                DEBUG("Получил статус от УУ\n");
                        }

                }
                return;

        }

        if (is_timer_elapsed(&conn_fsm_data->status_req_timer)) {
                reset_timer(&conn_fsm_data->status_req_timer);
                create_and_send_status_req_packet(conn_fsm_data);
                conn_fsm_data->state = WAIT_TX_COMPLETE;
                DEBUG("Посылаю запрос статуса к УУ\n");
                return;
        }

        if (is_timer_elapsed(&conn_fsm_data->standby_timer)) {
                conn_fsm_data->state = FREQ_FINDING;
                DEBUG("Соединение разорвано! Начинаю поиск рабочего канала\n");
        }

        return;
}

static inline void conn_fsm_wait_for_status_packet(struct conn_fsm_data_t* conn_fsm_data)
{
        if (is_timer_elapsed(&conn_fsm_data->status_answer_timer)) {
                conn_fsm_data->state = STANDBY;
                return;
        }

        struct rp_packet recieved_packet;

        if (nrf24l01_is_data_ready(conn_fsm_data->radio_nrf)) {
                nrf24l01_get_data(conn_fsm_data->radio_nrf, (uint8_t* ) &recieved_packet);

                if (packet_validate_checksum(&recieved_packet)) {
                        if (recieved_packet.packet_type == PACKET_TYPE_STATUS) {
                                reset_timer(&conn_fsm_data->beacon_timer);
                                conn_fsm_data->uu_status = (recieved_packet.flags == FLAG_ON_LINE_POWER) ?
                                        ON_LINE_POWER : OFF_LINE_POWER;
                                DEBUG("Получен ответ от УУ: %d\n", conn_fsm_data->uu_status);
                                
                                conn_fsm_data->state = STANDBY;
                        }

                }

        }



        return;
}

void conn_fsm_work(struct conn_fsm_data_t* conn_fsm_data)
{
        switch (conn_fsm_data->state) {
                case FREQ_FINDING: {
                        conn_fsm_freq_finding(conn_fsm_data);
                        break;
                }

                case FREQ_FINDING_CH_PROC: {
                        conn_fsm_freq_finding_ch_proc(conn_fsm_data);
                        break;
                }

                case FREQ_FINDING_WAIT_TX_COMPLETE: {
                        if (!nrf24l01_is_sending(conn_fsm_data->radio_nrf)) {
                                setup_timer(&conn_fsm_data->status_req_timer, CONFIG_STATUS_REQ_TIME);
                                reset_timer(&conn_fsm_data->status_req_timer);;

                                setup_timer(&conn_fsm_data->standby_timer, CONFIG_STANDBY_TIME);
                                reset_timer(&conn_fsm_data->standby_timer);
                                conn_fsm_data->state = STANDBY;
                        }
                        break;
                }

                case STANDBY: {
                        conn_fsm_standby(conn_fsm_data);
                        break;
                }

                case WAIT_TX_COMPLETE: {
                        if (!nrf24l01_is_sending(conn_fsm_data->radio_nrf)) {
                                setup_timer(&conn_fsm_data->status_answer_timer, CONFIG_ANSWER_WAIT_TIME);
                                reset_timer(&conn_fsm_data->status_answer_timer);
                                conn_fsm_data->state = WAIT_FOR_STATUS_PACKET;
                        }
                        break;
                }

                case WAIT_FOR_STATUS_PACKET: {
                        conn_fsm_wait_for_status_packet(conn_fsm_data);
                        break;
                }
        }
        return;
}

int8_t conn_fsm_is_uu_online_power(struct conn_fsm_data_t* conn_fsm_data)
{
        if (conn_fsm_data->uu_status == ON_LINE_POWER) {
                return 1;
        }
        else {
                return 0;
        }
}
