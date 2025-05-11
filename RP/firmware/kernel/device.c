/**
 * @file 
 * @brief Реализация подсистемы device
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include "device.h"

#include <string.h>
#include <stddef.h>

#define MAX_DEVICES 32

static const struct device *device_list[MAX_DEVICES];
static size_t device_count = 0;

int8_t device_register(const struct device *dev)
{
        if (device_count < MAX_DEVICES) {
                device_list[device_count] = dev;
                device_count++;
        } else {
                return -1;
        }

        return 0;
}

const struct device* device_get_by_name(const char* name)
{
        for (size_t i = 0; i < device_count; i++) {
                if (strcmp(device_list[i]->name, name) == 0)
                        return device_list[i];
        }

        return NULL;
}
