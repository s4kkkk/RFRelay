/**
 * @file 
 * @brief Конечный автомат, поддерживающий соединение с УУ
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef CONN_FSM_H
#define CONN_FSM_H

#include <stdint.h>

/**
 * @brief Объект, хранящий внутренние данные КА
 */
extern struct conn_fsm_data_t conn_fsm_data;

/**
 * @brief Функция инициализации
 */
void conn_fsm_init(struct conn_fsm_data_t* conn_fsm_data);

/**
 * @brief Функция рабочего такта КА
 */
void conn_fsm_work(struct conn_fsm_data_t* conn_fsm_data);

/**
 * @brief Получить состояние УУ.
 *
 * @return состояние УУ
 * 
 * @retval 0: 220В не на линии
 * @retval не 0: 220В на линии
 */
int8_t conn_fsm_is_uu_online_power(struct conn_fsm_data_t* conn_fsm_data);

#endif /* CONN_FSM_H */
