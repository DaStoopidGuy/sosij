CC := gcc
CFLAGS := -MMD -MP -ggdb

BUILD_DIR := build
TARGET := $(BUILD_DIR)/sosijc

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

# compile rule for each .c file
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean
run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
