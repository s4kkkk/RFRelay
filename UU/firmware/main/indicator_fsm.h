/**
 * @file 
 * @brief Конечный автомат, реализующий индикацию статуса посредсвом светодиодов
 *
 * Индикация реализована двумя светодиодами: красным и зеленым. 
 * Поиск рабочей частоты: одиночное мигание красного светодиода.
 * Соединение установлено: одиночное мигание зеленым светодиодом
 * Самостоятельная работа (без УУ): двойное мигание красным светодиодом
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INDICATOR_FSM_H
#define INDICATOR_FSM_H

/**
 * @brief Объект, хранящий внутренние данные КА
 */
extern struct indicator_fsm_data_t indicator_fsm_data;

/**
 * @brief Функция инициализации
 */
void indicator_fsm_init(struct indicator_fsm_data_t* indicator_fsm_data);

/**
 * @brief Функция рабочего такта КА
 */
void indicator_fsm_work(struct indicator_fsm_data_t* indicator_fsm_data);

enum indicator_status_t {
        /** Поиск рабочей частоты */
        INDICATOR_FREQ_FINDING,

        /** Соединение установлено */
        INDICATOR_CONN_ESTABLISHED,
};

/**
 * @brief Установка статуса
 *
 * @param status текущий статус РП
 */
void indicator_set_status(struct indicator_fsm_data_t* indicator_fsm_data, enum indicator_status_t status);

#endif /* INDICATOR_FSM_H */
