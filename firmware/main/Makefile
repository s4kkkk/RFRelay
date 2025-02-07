CURRENT_PATH := main

SRCS := $(wildcard $(CURRENT_PATH)/*.c)
OBJS := $(addprefix objs/, $(notdir $(SRCS:.c=.o)))

all: $(OBJS)

objs/%.o: $(CURRENT_PATH)/%.c
	mkdir -p objs
	$(CC) $(CFLAGS) -c -o $@ $<
