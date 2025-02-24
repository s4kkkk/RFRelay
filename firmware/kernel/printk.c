/**
 * @file 
 * @brief Реализация printk
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <printk.h>

#include <string.h>
#include <stdarg.h>

#include <drivers/uart.h>

#define PRINTK_BUFFER_LEN 64

static const struct device* printk_controller;
static char printk_buffer[PRINTK_BUFFER_LEN];


void set_printk_controller(const struct device *controller)
{
        printk_controller = controller;
        return;
}

static inline int print_int(char* out_str, size_t max_out, int val)
{
        if (!out_str)
                return 0;

        char buf[12];
        int i = 0;

        unsigned int abs_val = (val < 0) ? -val: val;

        /* поразрядно записываем цифры в буфер */
        do {
                char cur_char = '0' + (abs_val % 10);
                buf[i++] = cur_char;
                abs_val /= 10;
        } while (abs_val != 0);

        if (val < 0) {
                buf[i++] = '-';
        }

        if (i > max_out)
                return 0;

        /* записываем в строку */
        int j = 0;
        while (i != 0){
                out_str[j++] = buf[--i];
        }
        return j;
}

static size_t _vasnprintf(char* out_str, size_t size, const char* fmt, va_list va)
{
        if ( (!out_str) || !(fmt) || (size == 0))
                return 0;

        size_t i = 0;
        while (*fmt) {
                if (i >= (size - 1) )
                        break;

                if (*fmt == '%' && *(fmt + 1) == 'd') {
                        int num = va_arg(va, int);
                        i += print_int(out_str + i, (size - 1) - i, num);
                        fmt += 2;
                }
                else {
                        out_str[i++] = *(fmt++);
                }
        }
        out_str[i] = '\0';

        return i;
}


/* По умолчанию используется uart. Далее, возможно будут добавлены другие подсистемы.
 */
void printk(const char *fmt, ...)
{
        if (printk_controller == NULL)
                return;

        va_list va;
        va_start(va, fmt);
        _vasnprintf(printk_buffer, sizeof(printk_buffer), fmt, va);
        va_end(va);

        size_t msg_len = strlen(printk_buffer);

        while (!uart_is_transmitter_ready(printk_controller));
        uart_tx(printk_controller, printk_buffer, msg_len);
        uart_wait_for_tx_complete(printk_controller);

        return;
}
