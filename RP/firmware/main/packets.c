/**
 * @file 
 * @brief Реализация функций для работы с пакетами данных
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "packets.h"

#include <stddef.h>

static inline uint8_t get_crc8_sum(uint8_t *buffer, size_t size)
{
        uint8_t crc = 0;
        for (size_t i = 0; i < size; i++) {
                uint8_t data = buffer[i];
                for (int j = 8; j > 0; j--) {
                        crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
                        data >>= 1;
                }
        }
        return crc;
}

int8_t packet_validate_checksum(const struct rp_packet* packet)
{
        if (packet == NULL) {
                return -1;
        }

        uint8_t crc8_sum = get_crc8_sum((uint8_t* ) packet,
                                        sizeof(struct rp_packet) - 1);
        if (packet->check_sum == crc8_sum) {
                return 0;
        }
        else {
                return 1;
        }
}

int8_t packet_insert_checksum(struct rp_packet* packet)
{
        if (packet == NULL) {
                return -1;
        }

        uint8_t crc8_sum = get_crc8_sum((uint8_t* ) packet,
                                        sizeof(struct rp_packet) - 1);
        packet->check_sum = crc8_sum;

        return 0;
}
