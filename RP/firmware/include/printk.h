/**
 * @file 
 * @brief Функция для вывода системных сообщений
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#ifndef INCLUDE_PRINTK_H
#define INCLUDE_PRINTK_H

#include <device.h>

/**
 * @brief задать аппаратный контроллер, через который будут передаваться сообщения
 * (обычно UART). Конкретная используемая подсистема указывается в printk.c
 *
 * @param controller контроллер, через который будут выводиться данные.
 */
void set_printk_controller(const struct device* controller);

/**
 * @brief вывод системного сообщения
 *
 * Доступные спецификаторы:
 * - %d: вывод десятичного целого
 *
 * @param msg системное сообщение
 */
void printk(const char* fmt, ...);

#endif /* INCLUDE_PRINTK_H */
