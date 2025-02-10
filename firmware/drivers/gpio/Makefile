THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))
CURRENT_PATH := $(dir $(THIS_MAKEFILE))

SRCS := $(CURRENT_PATH)/gpio.c
OBJS := $(addprefix objs/, $(notdir $(SRCS:.c=.o)))

all: $(OBJS)

objs/%.o: $(CURRENT_PATH)/%.c
	mkdir -p objs
	$(CC) $(CFLAGS) -c -o $@ $<
