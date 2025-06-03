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

/**
 * @brief Проверка, нужно ли перейти в режим самостоятельной работы
 *
 * Самостоятельная работа - когда в независимости от состояния УУ РП включает реле только
 * в зависимости от состояния подключенного напрямую выключателя.
 * Этот режим необходим тогда, когда очень долго не было сеансов связи с УУ (возможно, он вышел из строя?)
 *
 * @return Необходим ли режим самостоятельной работы
 *
 * @retval 0: Необходимость отсутствует. Все работает штатно.
 * @retval не 0: Сеансов связи давно не было. Возможно, УУ вышел из строя. Необходима самостоятельная работа
 */
int8_t conn_fsm_need_self_work(struct conn_fsm_data_t* conn_fsm_data);

#endif /* CONN_FSM_H */
