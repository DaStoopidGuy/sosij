CC := clang
CFLAGS := -std=c17 -Wall -Wextra -Isrc/
RELEASE_CFLAGS := -Werror -Ofast
TEST_CFLAGS := -g -DDEBUG -fsanitize=address

SRC_DIR := src
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)

ALL_SRCS = $(SRCS)

RELEASE_OUTPUT := $(BUILD_DIR)/Sosij
TEST_OUTPUT := $(BUILD_DIR)/test

all: release

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

release: $(BUILD_DIR) $(RELEASE_OUTPUT)

$(RELEASE_OUTPUT): $(ALL_SRCS)
	@echo "Compiling release build..."
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) $^ $(LDFLAGS) -o $@
	@echo "Build successful: $(RELEASE_OUTPUT)"

.PHONY: test
test: $(BUILD_DIR) $(TEST_OUTPUT)

$(TEST_OUTPUT): $(ALL_SRCS)
	@echo "Compiling test build..."
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $^ $(LDFLAGS) -o $@
	@echo "Build successful: $(TEST_OUTPUT)"

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build files."CC := gcc
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
