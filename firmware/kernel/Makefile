THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))
CURRENT_PATH := $(dir $(THIS_MAKEFILE))

SRCS += $(wildcard $(CURRENT_PATH)/*.c)
OBJS += $(addprefix objs/, $(notdir $(SRCS:.c=.o)))

all: $(OBJS)

objs/%.o: $(CURRENT_PATH)/%.c
	mkdir -p objs
	$(CC) $(CFLAGS) -c -o $@ $<
